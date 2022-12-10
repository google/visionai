// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/packet_sender.h"

#include <memory>

#include "absl/memory/memory.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/synchronization/mutex.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/proto/util/net/grpc/connection_options.pb.h"
#include "visionai/streams/client/constants.h"
#include "visionai/streams/client/control.h"
#include "visionai/streams/client/descriptors.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/client/streaming_send_packets_grpc_client.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/streams/util/worker.h"
#include "visionai/util/net/grpc/client_connect.h"
#include "visionai/util/producer_consumer_queue.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
absl::Duration kDefaultWaitPeriod = absl::Milliseconds(100);
}  // namespace

// ----------------------------------------------------------------------------
// Grpc Sender Task
// ----------------------------------------------------------------------------

// The GrpcSenderTask contains the environment, data structures, and methods,
// that will allow a thread to carry out the work of sending packets to a
// channel.
class PacketSender::GrpcSenderTask {
 public:
  struct Options {
    std::string dataplane_address;
    std::string cluster_name;
    Channel channel;
    std::string sender;
    absl::Duration lease_term;
    ConnectionOptions connection_options;
  };

  static absl::StatusOr<std::unique_ptr<GrpcSenderTask>> Create(
      const Options& options) {
    auto task = std::make_unique<GrpcSenderTask>(options);
    task->submission_queue_ =
        std::make_shared<ProducerConsumerQueue<Packet>>(1);
    task->submission_reply_ =
        std::make_shared<ProducerConsumerQueue<absl::Status>>(1);
    // std::move needed to be compatible with OSS.
    return std::move(task);
  }

  // Submit a packet for sending.
  absl::Status SendAsync(Packet packet) {
    auto p = std::make_unique<Packet>(std::move(packet));
    if (!submission_queue_->TryPush(p)) {
      return absl::InternalError(
          "The GrpcSenderTask's submission queue is full.");
    }
    return absl::OkStatus();
  }

  // Wait for the result of SendAsync.
  absl::Status WaitForReply(absl::Duration timeout, bool cancel_if_timeout) {
    absl::Status status;
    if (!submission_reply_->TryPop(status, timeout)) {
      if (!cancel_if_timeout) {
        return absl::UnavailableError("The send is still in progress.");
      } else {
        Cancel();
        return absl::CancelledError("The Send request has timed out.");
      }
    }
    return status;
  }

  // The main method that a background worker should execute. It blocks until a
  // cancellation or an error.
  //
  // The worker waits for Packet submissions, and then uses a
  // StreamingSendPacketsGrpcClient to send it to the server.
  absl::Status Run() {
    // Create the StreamingSendPacketsGrpcClient.
    StreamingSendPacketsGrpcClient::Options options;
    options.target_address = options_.dataplane_address;
    options.cluster_name = options_.cluster_name;
    options.channel = options_.channel;
    options.sender = options_.sender;
    options.lease_term = options_.lease_term;
    options.connection_options = options_.connection_options;
    VAI_ASSIGN_OR_RETURN(grpc_send_client_,
                     StreamingSendPacketsGrpcClient::Create(options),
                     _ << "while creating the StreamingSendPacketsGrpcClient");

    // Main loop. Can only exit by a cancellation or a non-OK return.
    while (!cancel_notification_.HasBeenNotified()) {
      // Get a Packet from the caller of `Send`.
      Packet p;
      if (!submission_queue_->TryPop(p, kDefaultQueuePopTimeout)) {
        continue;
      }

      // Send the packet into the `Channel`.
      auto status = grpc_send_client_->Send(std::move(p));

      // Reply to the submitter.
      auto reply = std::make_unique<absl::Status>(status);
      if (!submission_reply_->TryPush(reply)) {
        return absl::InternalError("The GrpcSenderTask's reply queue is full.");
      }

      // Exit with an error status if not-OK.
      if (!status.ok()) {
        return status;
      }
    }
    grpc_send_client_->Cancel();
    return absl::CancelledError("Received a cancellation request.");
  }

  void Cancel() {
    if (!cancel_notification_.HasBeenNotified()) {
      cancel_notification_.Notify();
    }
  };

  explicit GrpcSenderTask(const Options& options) : options_(options){};
  ~GrpcSenderTask() = default;
  GrpcSenderTask(const GrpcSenderTask&) = delete;
  GrpcSenderTask& operator=(const GrpcSenderTask&) = delete;

 private:
  const Options options_;

  absl::Notification cancel_notification_;
  std::unique_ptr<StreamingSendPacketsGrpcClient> grpc_send_client_;
  std::shared_ptr<ProducerConsumerQueue<Packet>> submission_queue_;
  std::shared_ptr<ProducerConsumerQueue<absl::Status>> submission_reply_;
};

absl::StatusOr<std::unique_ptr<PacketSender::WorkContext>>
PacketSender::BuildGrpcSenderWorkContext() {
  // Create the `GrpcSenderTask`. Its `Run` function will become the task of
  // this `WorkContext`.
  GrpcSenderTask::Options options;
  VAI_ASSIGN_OR_RETURN(
      auto dataplane_address,
      GetClusterEndpoint(options_.cluster_selection),
      _ << "while getting the cluster dataplane endpoint.");
  options.dataplane_address = dataplane_address;
  VAI_ASSIGN_OR_RETURN(options.cluster_name,
                   ClusterNameFrom(options_.cluster_selection),
                   _ << "while getting the cluster name.");
  options.channel = options_.channel;
  options.sender = options_.sender;
  options.lease_term = options_.grace_period;
  options.connection_options = DefaultConnectionOptions();
  options.connection_options.mutable_ssl_options()->set_use_insecure_channel(
      options_.cluster_selection.use_insecure_channel());
  VAI_ASSIGN_OR_RETURN(grpc_sender_task_, GrpcSenderTask::Create(options),
                   _ << "while creating the grpc sender task");

  // Create the work context.
  auto ctx = std::make_unique<WorkContext>();
  std::string task_name = "grpc sender";
  ctx->task_name = task_name;
  ctx->task = [this, task_name]() {
    auto return_status = grpc_sender_task_->Run();
    if (return_status.ok()) {
      return_status = absl::InternalError(
          absl::StrFormat("The \"%s\" task completed with an OK status; it "
                          "should only return with "
                          "an error status or continue to run.",
                          task_name));
    }
    MaybeUpdateStrongestErrorStatus(return_status);
    return return_status;
  };
  ctx->canceller = [this]() {
    grpc_sender_task_->Cancel();
    return absl::OkStatus();
  };
  ctx->finalize_timeout = kDefaultGrpcSendWorkerFinalizationTimeout;
  ctx->worker = std::make_unique<streams_internal::Worker>();
  return std::move(ctx);
}

// ----------------------------------------------------------------------------
// PacketSender Implementation
// ----------------------------------------------------------------------------
//
// The PacketSender goes through the following during initialization:
//
// 1. Build WorkContexts.
//
//    Each task that will be running in a background thread is encapsulated
//    within a WorkContext. It contains not only the task itself, but also the
//    Worker that will be running it.
//
//    Currently, there is only one work context for the grpc sender. There used
//    to also be a lease renewer, but that is removed for now.
//
//    TODO(b/247929951): Clean up and simplify the PacketSender.
//
// 2. Start the workers.
//
//    The main thread will complete the initialization step after starting all
//    the workers within each context. Control will then be passed back to the
//    user of PacketSender where Sends will now be possible.
//
// The background workers will continue to run as long as they don't encounter
// an error or receive a cancellation. The user of `PacketSender` will detect
// this the next time they call `Send`.
//
// All background threads will be sent a cancel request when the caller of
// `Send` detects an error, or when the destructor runs.

PacketSender::PacketSender(const Options& options) : options_(options) {}

absl::Status PacketSender::ValidateOptions() {
  VAI_RETURN_IF_ERROR(
      ValidateClusterSelection(options_.cluster_selection));
  if (options_.channel.event_id.empty()) {
    return absl::InvalidArgumentError(
        "The `event_id` of the channel must be specified.");
  }
  if (options_.channel.stream_id.empty()) {
    return absl::InvalidArgumentError(
        "The `stream_id` of of the channel must be specified.");
  }
  if (options_.sender.empty()) {
    return absl::InvalidArgumentError("The `sender` must be specified.");
  }
  return absl::OkStatus();
}

absl::Status PacketSender::BuildWorkContexts() {
  VAI_ASSIGN_OR_RETURN(auto grpc_sender_ctx, BuildGrpcSenderWorkContext(),
                   _ << "while building the grpc sender context");
  work_contexts_.push_back(std::move(grpc_sender_ctx));
  return absl::OkStatus();
}

absl::Status PacketSender::StartWork() {
  for (auto& ctx : work_contexts_) {
    VAI_RETURN_IF_ERROR(ctx->worker->Work(ctx->task))
        << "while starting work for \"" << ctx->task_name << "\"";
  }
  return absl::OkStatus();
}

absl::Status PacketSender::Initialize() {
  VAI_RETURN_IF_ERROR(ValidateOptions()) << "while validating options";
  VAI_RETURN_IF_ERROR(BuildWorkContexts()) << "while building work contexts";
  VAI_RETURN_IF_ERROR(StartWork()) << "while starting work";
  return absl::OkStatus();
}

absl::StatusOr<std::unique_ptr<PacketSender>> PacketSender::Create(
    const Options& options) {
  auto packet_sender = absl::WrapUnique(new PacketSender(options));
  VAI_RETURN_IF_ERROR(packet_sender->Initialize());
  return std::move(packet_sender);
}

// Request the cancellation of all workers.
//
// Note that Worker::Cancel invokes the callback on each worker at most once,
// and only on those that may still be running.
void PacketSender::CancelWork() {
  for (auto& ctx : work_contexts_) {
    ctx->worker->Cancel(ctx->canceller);
    absl::Status return_status;
    auto s =
        ctx->worker->GetReturnStatus(ctx->finalize_timeout, &return_status);
    if (!s.ok()) {
      return_status = absl::CancelledError(
          absl::StrFormat("The cancellation of \"%s\" has been requested "
                          "but it might still be running",
                          ctx->task_name));
    }
    MaybeUpdateStrongestErrorStatus(return_status);
  }
}

PacketSender::~PacketSender() { CancelWork(); }

// Update the strongest error status if the given candidate status is stronger
// than the existing strongest error status.
//
// OK < CANCELLED < others.
void PacketSender::MaybeUpdateStrongestErrorStatus(
    const absl::Status& candidate) {
  absl::MutexLock l(&strongest_error_status_mu_);
  if (candidate.ok()) {
    return;
  } else if (absl::IsCancelled(candidate)) {
    if (strongest_error_status_.ok()) {
      strongest_error_status_ = candidate;
    }
  } else {
    strongest_error_status_ = candidate;
  }
}

absl::Status PacketSender::GetStrongestErrorStatus() {
  absl::MutexLock l(&strongest_error_status_mu_);
  return strongest_error_status_;
}

absl::Status PacketSender::Send(Packet p) {
  return Send(std::move(p), absl::InfiniteDuration());
}

absl::Status PacketSender::Send(Packet p, absl::Duration timeout) {
  VAI_RETURN_IF_ERROR(GetStrongestErrorStatus());
  VAI_RETURN_IF_ERROR(grpc_sender_task_->SendAsync(std::move(p)));
  absl::Status return_status;
  while (true) {
    bool is_last = timeout < kDefaultWaitPeriod;
    auto wait_duration = is_last ? timeout : kDefaultWaitPeriod;
    auto status = grpc_sender_task_->WaitForReply(wait_duration, is_last);
    if (status.ok()) {
      return_status = absl::OkStatus();
      break;
    } else {
      if (!absl::IsUnavailable(status)) {
        CancelWork();
        MaybeUpdateStrongestErrorStatus(status);
        return_status = GetStrongestErrorStatus();
        break;
      } else {
        return_status = GetStrongestErrorStatus();
        if (!return_status.ok()) {
          break;
        }
      }
    }
    timeout -= kDefaultWaitPeriod;
  }
  return return_status;
}

}  // namespace visionai
