// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/util/packet_loop_runner.h"

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/strings/match.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/streams/client/constants.h"
#include "visionai/util/telemetry/metrics/stats.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
constexpr absl::Duration kCommitPacketTimeout = absl::Seconds(5);
constexpr absl::Duration kReceivePacketInterval = absl::Seconds(10);

#define METRIC_RECEIVE_PACKET_FROM_STREAM(option, packet)            \
  capture_received_packets_from_stream_total()                       \
      .Add({{"project_id", option.cluster_selection.project_id()},   \
            {"location_id", option.cluster_selection.location_id()}, \
            {"cluster_id", option.cluster_selection.cluster_id()},   \
            {"stream_id", option.channel.stream_id},                 \
            {"event_id", option.channel.event_id},                   \
            {"lesse", option.lessee}})                               \
      .Increment();                                                  \
  capture_received_bytes_from_stream_total()                         \
      .Add({{"project_id", option.cluster_selection.project_id()},   \
            {"location_id", option.cluster_selection.location_id()}, \
            {"cluster_id", option.cluster_selection.cluster_id()},   \
            {"stream_id", option.channel.stream_id},                 \
            {"event_id", option.channel.event_id},                   \
            {"lesse", option.lessee}})                               \
      .Increment(packet.ByteSizeLong());
}  // namespace

void PacketLoopRunner::CommitPacketOffset(int64_t offset) {
  LOG(INFO) << "Commiting packet offset " << offset;
  {
    absl::MutexLock lock(&local_commit_offset_mu_);
    local_commit_offset_ = offset;
  }
  if (!packet_receiver_) {
    LOG(WARNING) << "Packet receiver unitialized";
    return;
  }
  bool commit_ok;
  if (!packet_receiver_->Commit(kCommitPacketTimeout, offset, &commit_ok)) {
    LOG(ERROR) << absl::StrFormat("Packet Commit Timeout (offset %d)", offset);
    return;
  }
  if (!commit_ok) {
    LOG(ERROR) << absl::StrFormat("Packet Commit Failed (offset %d)", offset);
  }
}

void PacketLoopRunner::OnReceivePacket(Packet p) {
  auto write_status = event_writer_->Write(p);
  if (!write_status.ok()) {
    if (absl::IsUnavailable(write_status)) {
      LOG(INFO) << "Event writer reached full capacity - backoff 1s.";
      absl::SleepFor(absl::Seconds(1));
      write_status = event_writer_->Write(p);
      if (!write_status.ok()) {
        LOG(WARNING) << "Event writer reached full capacity - dropping. "
                     << write_status;
      }
    } else {
      LOG(ERROR) << "Event writer faile to write: " << write_status;
    }
  }
}

void PacketLoopRunner::Finalize() {
  if (event_writer_) {
    auto close_status = event_writer_->Close();
    if (!close_status.ok()) {
      LOG(ERROR) << "EventWriter closed with error " << close_status;
    };
    event_writer_.reset();
  }
  packet_receiver_.reset();
}

absl::Status PacketLoopRunner::Run() {
  // TODO(annikaz): Change the receive mode to enum representation.
  if (options_.packet_receiver_options.receive_mode == "controlled") {
    return RunControlledMode();
  }
  return RunEagerMode();
}

absl::Status PacketLoopRunner::RunEagerMode() {
  std::string event_id = options_.packet_receiver_options.channel.event_id;

  while (!is_canceled_.HasBeenNotified()) {
    // TODO(annikaz): Consider parallelize the initialization if the latency is
    // significant.
    if (packet_receiver_ == nullptr) {
      LOG(INFO) << "Initializing PacketReceiver";
      VAI_ASSIGN_OR_RETURN(
          packet_receiver_,
          options_.packet_receiver_factory(options_.packet_receiver_options),
          _.LogError() << " While creating the PacketReceiver");
    }
    if (event_writer_ == nullptr) {
      LOG(INFO) << "Initializing EventWriter";
      VAI_ASSIGN_OR_RETURN(event_writer_,
                       options_.event_writer_factory(event_id, nullptr),
                       _.LogError() << " While creating the EventWriter");
    }
    // handle error.
    Packet p;
    auto s = packet_receiver_->Receive(kReceivePacketInterval, &p);
    if (!s.ok()) {
      LOG(ERROR) << absl::StrFormat(
          "Error from packet receiver (event: %s): %s", event_id, s.ToString());
      if (absl::IsNotFound(s) &&
          absl::StrContains(s.message(), kPacketReceiverErrMsgPrefix)) {
        // Continue to poll from the same packet receiver.
        continue;
      }
      if (absl::IsOutOfRange(s)) {
        // Work done.
        if (options_.event_commit_callback) {
          LOG(INFO) << "Committing event " << event_id;
          options_.event_commit_callback(options_.current_event.offset());
        }
        break;
      }
      // Reset the packet receiver to retry.
      packet_receiver_.reset();
      continue;
    }
    OnReceivePacket(p);
  }
  Finalize();
  return absl::OkStatus();
}

absl::Status PacketLoopRunner::RunControlledMode() {
  std::string event_id = options_.packet_receiver_options.channel.event_id;

  OffsetCommitCallback callback = [this](int64_t offset) {
    return CommitPacketOffset(offset);
  };
  Packet p;
  while (!is_canceled_.HasBeenNotified()) {
    if (packet_receiver_ == nullptr) {
      LOG(INFO) << "Initializing PacketReceiver";
      VAI_ASSIGN_OR_RETURN(
          packet_receiver_,
          options_.packet_receiver_factory(options_.packet_receiver_options),
          _.LogError() << " While creating the PacketReceiver");
    }
    if (event_writer_ == nullptr) {
      LOG(INFO) << "Initializing EventWriter";
      VAI_ASSIGN_OR_RETURN(event_writer_,
                       options_.event_writer_factory(event_id, callback),
                       _.LogError() << " While creating the EventWriter");
    }
    bool receive_ok = true;
    if (!packet_receiver_->Receive(kReceivePacketInterval, &p, &receive_ok)) {
      // timeout
      continue;
    }

    if (!receive_ok) {
      packet_receiver_->CommitsDone();
      auto receive_status = packet_receiver_->Finish();

      LOG(INFO) << "Packet receiver finished with status " << receive_status;
      if (absl::IsOutOfRange(receive_status)) {
        LOG(INFO) << "Committing event " << event_id;
        if (options_.event_commit_callback) {
          options_.event_commit_callback(options_.current_event.offset());
        }
        break;
      } else {
        packet_receiver_.reset();
        continue;
      }
    }
    {
      absl::MutexLock lock(&local_commit_offset_mu_);
      if (GetOffset(p) <= local_commit_offset_) {
        LOG_EVERY_N(INFO, 20)
            << "Packet already processed, offset " << GetOffset(p);
        continue;
      }
    }
    OnReceivePacket(p);
  }
  Finalize();
  return absl::OkStatus();
}
}  // namespace visionai