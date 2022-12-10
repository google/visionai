// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/packet_receiver.h"

#include <cstdint>
#include <memory>

#include "google/cloud/visionai/v1/streaming_resources.pb.h"
#include "absl/container/flat_hash_map.h"
#include "absl/functional/bind_front.h"
#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/constants.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/read_write_channel.h"
#include "visionai/streams/client/streaming_receive_packets_grpc_v1_client.h"
#include "visionai/streams/client/worker_v1.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/ring_buffer.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

using google::cloud::visionai::v1::ReceivePacketsResponse;

using visionai::streams_internal::Reader;
using visionai::streams_internal::ReaderArray;
using visionai::streams_internal::ReadWriteChannel;
using visionai::streams_internal::SelectForWriteEvent;
using visionai::streams_internal::WorkerV1;
using visionai::streams_internal::Writer;

constexpr absl::Duration kOffsetCommitterReadTimeout = absl::Seconds(1);
constexpr absl::Duration kPacketTransferrerReadTimeout = absl::Seconds(1);

}  // namespace
// ----------------------------------------------------------------------------
// OffsetCommitterTask
// ----------------------------------------------------------------------------

absl::Status PacketReceiver::OffsetCommitterTask(
    Writer<absl::Status>* status_writer, Reader<int64_t>* offset_reader) {
  auto return_with_status = [status_writer](absl::Status s) -> absl::Status {
    bool ok;
    status_writer->Write(absl::InfiniteDuration(), s, &ok);
    status_writer->Close();
    return s;
  };

  absl::Status final_status = absl::OkStatus();
  bool write_channel_closed = false;
  while (!offset_committer_cancelled_->HasBeenNotified()) {
    // Wait for a commit request from the client.
    int64_t offset;
    bool ok;
    if (!offset_reader->Read(kOffsetCommitterReadTimeout, &offset, &ok)) {
      continue;
    }

    // Case 1: The client has closed the channel (`CommitsDone`).
    if (!ok) {
      grpc_client_->WritesDone();
      write_channel_closed = true;
      break;
    }

    // Case 2: Got a new offset to commit.
    if (!grpc_client_->WriteCommit(offset)) {
      write_channel_closed = true;
      break;
    }
  }

  // Forcefully close the write channel if necessary.
  if (!write_channel_closed) {
    grpc_client_->Cancel();
  }

  return return_with_status(final_status);
}

absl::Status PacketReceiver::CancelOffsetCommitterTask() {
  if (!offset_committer_cancelled_->HasBeenNotified()) {
    offset_committer_cancelled_->Notify();
  }
  return absl::OkStatus();
}

// ----------------------------------------------------------------------------
// PacketTransferrerTask
// ----------------------------------------------------------------------------

absl::Status PacketReceiver::PacketTransferrerTask(
    Writer<absl::Status>* status_writer,
    Writer<std::shared_ptr<Packet>>* packet_writer) {
  auto return_with_status = [status_writer,
                             packet_writer](absl::Status s) -> absl::Status {
    bool ok;
    status_writer->Write(absl::InfiniteDuration(), s, &ok);
    status_writer->Close();
    packet_writer->Close();
    return s;
  };

  // Start a worker just to perform the blocking grpc client `Read`.
  ReadWriteChannel<std::shared_ptr<ReceivePacketsResponse>> response_channel(1);
  auto response_writer = response_channel.writer();
  auto response_reader_cancelled = std::make_shared<absl::Notification>();
  auto response_reader = std::make_unique<WorkerV1>(
      [this, response_reader_cancelled, response_writer]() {
        ReceivePacketsResponse response;
        while (!response_reader_cancelled->HasBeenNotified()) {
          if (!grpc_client_->Read(&response)) {
            break;
          } else {
            auto response_ptr =
                std::make_shared<ReceivePacketsResponse>(std::move(response));
            bool ok;
            response_writer->Write(absl::InfiniteDuration(), response_ptr, &ok);
            if (!ok) {
              break;
            }
          }
        }
        response_writer->Close();
        return absl::OkStatus();
      },
      [response_reader_cancelled]() {
        if (!response_reader_cancelled->HasBeenNotified()) {
          response_reader_cancelled->Notify();
        }
        return absl::OkStatus();
      });

  // Repeatedly poll responses from the response reader.
  absl::Status final_status = absl::OkStatus();
  bool read_channel_closed = false;
  while (!packet_transferrer_cancelled_->HasBeenNotified()) {
    // Poll for up to the heartbeat interval and grace period for any kind of
    // server communications.
    std::shared_ptr<ReceivePacketsResponse> response_ptr = nullptr;
    bool stop_working = false;
    bool missed_heartbeat_interval = true;
    bool got_receive_packets_response = false;
    for (absl::Duration heartbeat_countdown =
             options_.advanced.heartbeat_interval +
             options_.advanced.heartbeat_grace_period;
         heartbeat_countdown >= absl::ZeroDuration();
         heartbeat_countdown -= kPacketTransferrerReadTimeout) {
      bool timed_out_polling = !response_channel.reader()->Read(
          kPacketTransferrerReadTimeout, &response_ptr,
          &got_receive_packets_response);
      if (timed_out_polling) {
        if (packet_transferrer_cancelled_->HasBeenNotified()) {
          stop_working = true;
          break;
        }
      } else {
        missed_heartbeat_interval = false;
        break;
      }
    }
    if (stop_working) {
      break;
    }

    // Case 1: Server misses heartbeat.
    if (missed_heartbeat_interval) {
      LOG(ERROR)
          << "The server has missed a packet heartbeat; we will cancel the RPC";
      break;
    }

    // Case 2: Response reader closed.
    // This means the read channel is closed.
    if (!got_receive_packets_response) {
      read_channel_closed = true;
      break;
    }

    // Case 3: Got a packet. Deliver it to the caller.
    if (response_ptr->has_packet()) {
      auto packet_ptr =
          std::make_shared<Packet>(std::move(*response_ptr->mutable_packet()));
      bool ok;
      packet_writer->Write(absl::InfiniteDuration(), packet_ptr, &ok);

      // This indicates that the caller has cancelled.
      if (!ok) {
        break;
      }
    }

    // Case 4: Got a heartbeat. Just ignore.
    if (response_ptr->control().has_heartbeat()) {
      continue;
    }

    // Case 5: Got a writes done request.
    //
    // To the caller, the read stream has logically ended. They must be
    // unblocked to either complete writes or cancel.
    //
    // This thread will now wait for one of those conditions to determine the
    // status of the read channel.
    if (response_ptr->control().has_writes_done_request()) {
      // Unblock the caller so they may go complete their writes.
      packet_writer->Close();

      // Wait for the caller to act.
      //
      // They will either complete their writes or eventually cancel.
      do {
        bool read_ok = false;
        if (!response_channel.reader()->Read(absl::Milliseconds(100),
                                             &response_ptr, &read_ok)) {
          continue;
        } else {
          if (!read_ok) {
            read_channel_closed = true;
          } else {
            final_status = absl::InternalError(
                "Unexpectedly got a response frorm upstream after getting a "
                "writes done request");
          }
          break;
        }
      } while (!packet_transferrer_cancelled_->WaitForNotificationWithTimeout(
          absl::Seconds(1)));
    }
  }

  // Forcefully close the channel if necessary.
  // This will also unblock the response reader.
  if (!read_channel_closed) {
    response_reader->Cancel();
    response_channel.CancelWriters();
    grpc_client_->Cancel();
  }
  response_reader->Join();

  return return_with_status(final_status);
}

absl::Status PacketReceiver::CancelPacketTransferrerTask() {
  if (!packet_transferrer_cancelled_->HasBeenNotified()) {
    packet_transferrer_cancelled_->Notify();
  }
  return absl::OkStatus();
}

// ----------------------------------------------------------------------------
// MainTask
// ----------------------------------------------------------------------------

absl::Status PacketReceiver::MainTask(
    Writer<absl::Status>* status_writer,
    Writer<std::shared_ptr<Packet>>* packet_writer,
    Reader<int64_t>* offset_reader) {
  auto return_with_status = [this,
                             status_writer](absl::Status s) -> absl::Status {
    bool ok;
    status_writer->Write(absl::InfiniteDuration(), s, &ok);
    status_writer->Close();
    commit_channel_->CancelWriters();
    return s;
  };

  // Start workers.
  ReadWriteChannel<absl::Status> offset_committer_status_channel(1);
  auto offset_committer = std::make_unique<WorkerV1>(
      absl::bind_front(&PacketReceiver::OffsetCommitterTask, this,
                       offset_committer_status_channel.writer(), offset_reader),
      absl::bind_front(&PacketReceiver::CancelOffsetCommitterTask, this));

  ReadWriteChannel<absl::Status> packet_transferrer_status_channel(1);
  auto packet_transferrer = std::make_unique<WorkerV1>(
      absl::bind_front(&PacketReceiver::PacketTransferrerTask, this,
                       packet_transferrer_status_channel.writer(),
                       packet_writer),
      absl::bind_front(&PacketReceiver::CancelPacketTransferrerTask, this));

  // Wait and collect final statuses.
  std::vector<Reader<absl::Status>*> status_readers = {
      offset_committer_status_channel.reader(),
      packet_transferrer_status_channel.reader()};
  absl::Status return_status = absl::OkStatus();
  std::vector<std::unique_ptr<absl::Status>> final_statuses(
      status_readers.size());

  // Phase 1: Wait for the first non-OK status. Collect any OK statuses that
  // arrive earlier too.
  for (int i = 0; i < status_readers.size(); ++i) {
    // Only wait on those that don't yet have a status.
    ReaderArray<absl::Status> readers;
    absl::flat_hash_map<int, int> pick_idx_map;
    for (size_t j = 0; j < status_readers.size(); ++j) {
      if (final_statuses[j] == nullptr) {
        pick_idx_map[readers.size()] = j;
        readers.push_back(status_readers[j]);
      }
    }

    int picked = pick_idx_map[SelectForWriteEvent(readers)];

    // Collect the status.
    absl::Status s;
    bool ok = false;
    status_readers[picked]->Read(absl::InfiniteDuration(), &s, &ok);
    final_statuses[picked] = std::make_unique<absl::Status>(s);

    // Got the first non-OK status.
    if (!s.ok()) {
      if (return_status.ok()) {
        return_status = s;
      }
      break;
    }
  }

  // Phase 2: Cancel any remaining workers and collect their statuses.

  // Cancel and join all workers. Completed workers wouldn't be affected.
  offset_committer->Cancel();
  packet_transferrer->Cancel();
  offset_committer->Join();
  packet_transferrer->Join();

  // Collect remaining statuses.
  for (size_t i = 0; i < final_statuses.size(); ++i) {
    if (final_statuses[i] == nullptr) {
      absl::Status s;
      bool ok = false;
      status_readers[i]->Read(absl::InfiniteDuration(), &s, &ok);
      final_statuses[i] = std::make_unique<absl::Status>(s);
    }
  }

  // If all went well, collect the RPC status and use that as the return.
  //
  // Note: `Finish` will be called even if this branch doesn't execute. The only
  // difference is whether the status is logged to screen or returned to the
  // caller.
  if (return_status.ok()) {
    auto s = grpc_client_->Finish(&return_status);
    if (!s.ok()) {
      return return_with_status(absl::InternalError(
          absl::StrFormat("Encountered an unexpected error while trying to "
                          "`Finish` the grpc client: %s",
                          s.message())));
    }
  }

  return return_with_status(return_status);
}

absl::Status PacketReceiver::CancelMainTask() {
  CancelOffsetCommitterTask().IgnoreError();
  CancelPacketTransferrerTask().IgnoreError();
  receive_channel_->CancelWriters();
  commit_channel_->CancelWriters();
  return absl::OkStatus();
}

// ----------------------------------------------------------------------------
// PacketReceiver
// ----------------------------------------------------------------------------

absl::StatusOr<std::unique_ptr<PacketReceiver>> PacketReceiver::Create(
    const Options& options) {
  auto packet_receiver = absl::WrapUnique(new PacketReceiver(options));
  VAI_RETURN_IF_ERROR(packet_receiver->Initialize());
  return std::move(packet_receiver);
}

PacketReceiver::PacketReceiver(const Options& options) : options_(options) {}

absl::Status PacketReceiver::Initialize() {
  // Complete handshakes with the server and initialize shared data
  // structures.
  VAI_RETURN_IF_ERROR(CompleteOptionsWithDefaults());
  VAI_RETURN_IF_ERROR(CreateGrpcClient()) << "while creating the grpc client";

  // Setup channels and run the main task.
  main_status_channel_ = std::make_shared<ReadWriteChannel<absl::Status>>(1);
  receive_channel_ =
      std::make_shared<ReadWriteChannel<std::shared_ptr<Packet>>>(1);
  commit_channel_ = std::make_shared<ReadWriteChannel<int64_t>>(1);

  offset_committer_cancelled_ = std::make_shared<absl::Notification>();
  packet_transferrer_cancelled_ = std::make_shared<absl::Notification>();

  main_ = std::make_unique<WorkerV1>(
      absl::bind_front(&PacketReceiver::MainTask, this,
                       main_status_channel_->writer(),
                       receive_channel_->writer(), commit_channel_->reader()),
      absl::bind_front(&PacketReceiver::CancelMainTask, this));
  return absl::OkStatus();
}

bool PacketReceiver::Receive(absl::Duration timeout, Packet* packet, bool* ok) {
  std::shared_ptr<Packet> p;
  if (!receive_channel_->reader()->Read(timeout, &p, ok)) {
    return false;
  }

  if (*ok) {
    *packet = std::move(*p);
  }
  return true;
}

absl::Status PacketReceiver::Receive(absl::Duration timeout, Packet* packet) {
  bool read_ok = false;

  // Case 1: Timeout. Signal not found.
  if (!Receive(timeout, packet, &read_ok)) {
    return absl::NotFoundError(absl::StrFormat(
        "%s No packets are available from upstream at this time",
        kPacketReceiverErrMsgPrefix));
  }

  // Case 2: Got a packet. Deliver it too the caller.
  if (read_ok) {
    return absl::OkStatus();
  }

  // Case 3: The read channel terminated. Close the write channel and return
  // the RPC status to the caller.
  this->CommitsDone();
  return this->Finish();
}

absl::Status PacketReceiver::Receive(Packet* packet) {
  return Receive(absl::InfiniteDuration(), packet);
}

bool PacketReceiver::Commit(absl::Duration timeout, int64_t offset, bool* ok) {
  return commit_channel_->writer()->Write(timeout, offset, ok);
}

void PacketReceiver::CommitsDone() { commit_channel_->writer()->Close(); }

void PacketReceiver::Cancel() {
  if (main_ != nullptr && main_->IsJoinable()) {
    main_->Cancel();
    main_->Join();
  }
}

absl::Status PacketReceiver::Finish() {
  absl::Status final_status;
  bool ok;
  main_status_channel_->reader()->Read(absl::InfiniteDuration(), &final_status,
                                       &ok);
  return final_status;
}

PacketReceiver::~PacketReceiver() { Cancel(); }

// ----------------------------------------------------------------------------
// Application helper functions
// ----------------------------------------------------------------------------

absl::Status PacketReceiver::CompleteOptionsWithDefaults() {
  if (options_.advanced.grace_period == absl::ZeroDuration()) {
    options_.advanced.grace_period = kDefaultLeaseDuration;
  }

  if (options_.advanced.heartbeat_interval == absl::ZeroDuration() ||
      options_.advanced.heartbeat_interval == absl::InfiniteDuration()) {
    options_.advanced.heartbeat_interval = kDefaultServerHeartbeatInterval;
  }

  if (options_.advanced.heartbeat_grace_period == absl::ZeroDuration() ||
      options_.advanced.heartbeat_grace_period == absl::InfiniteDuration()) {
    options_.advanced.heartbeat_grace_period = kDefaultHeartbeatGracePeriod;
  }

  return absl::OkStatus();
}

absl::Status PacketReceiver::ValidateChannelOptions() {
  if (options_.channel.event_id.empty()) {
    return absl::InvalidArgumentError("Given an empty event id");
  }
  if (options_.channel.stream_id.empty()) {
    return absl::InvalidArgumentError("Given an empty stream id");
  }
  return absl::OkStatus();
}

absl::Status PacketReceiver::CreateGrpcClient() {
  StreamingReceivePacketsGrpcV1Client::Options options;

  VAI_RETURN_IF_ERROR(
      ValidateClusterSelection(options_.cluster_selection));
  VAI_ASSIGN_OR_RETURN(
      options.target_address,
      GetClusterEndpoint(options_.cluster_selection),
      _ << "while getting the cluster dataplane endpoint.");

  VAI_ASSIGN_OR_RETURN(options.cluster_name,
                   ClusterNameFrom(options_.cluster_selection),
                   _ << "while getting the cluster name.");

  VAI_RETURN_IF_ERROR(ValidateChannelOptions());
  options.channel = options_.channel;
  options.receiver = options_.lessee;
  options.lease_term = options_.advanced.grace_period;
  options.receive_mode = options_.receive_mode;
  options.advanced.controlled_mode_options.starting_logical_offset =
      options_.advanced.controlled_mode_options.starting_logical_offset;
  options.advanced.controlled_mode_options.fallback_starting_offset =
      options_.advanced.controlled_mode_options.fallback_starting_offset;

  options.advanced.heartbeat_interval = options_.advanced.heartbeat_interval;
  options.advanced.writes_done_grace_period =
      options_.advanced.writes_done_grace_period;

  options.advanced.connection_options = DefaultConnectionOptions();
  options.advanced.connection_options.mutable_ssl_options()
      ->set_use_insecure_channel(
          options_.cluster_selection.use_insecure_channel());

  VAI_ASSIGN_OR_RETURN(grpc_client_,
                   StreamingReceivePacketsGrpcV1Client::Create(options));

  return absl::OkStatus();
}

}  // namespace visionai
