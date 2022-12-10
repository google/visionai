// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/util/event_loop_runner.h"

#include <cstdint>

#include "visionai/streams/client/event_update.h"
#include "visionai/util/telemetry/metrics/stats.h"

namespace visionai {

namespace {
constexpr absl::Duration kCommitEventTimeout = absl::Seconds(5);
constexpr absl::Duration kReceiveEventInterval = absl::Seconds(10);

#define METRIC_RECEIVE_EVENT(cluster_selection, stream_id, receiver) \
  capture_received_events_total()                                    \
      .Add({{"project_id", cluster_selection.project_id()},          \
            {"location_id", cluster_selection.location_id()},        \
            {"cluster_id", cluster_selection.cluster_id()},          \
            {"stream_id", stream_id},                                \
            {"lesse", receiver}})                                    \
      .Increment();
}  // namespace

void EventLoopRunner::OnReceiveEvent(EventUpdate event_update) {
  LOG(INFO) << "************ Processing event " << event_update.event()
            << "; offset " << event_update.offset() << " ************";

  if (packet_loop_runner_) {
    packet_loop_runner_->Cancel();
    t_.join();
    packet_loop_runner_.reset();
  }
  PacketLoopRunner::Options packet_loop_runner_opts;
  packet_loop_runner_opts.packet_receiver_options =
      options_.packet_receiver_options;
  packet_loop_runner_opts.packet_receiver_factory =
      options_.packet_receiver_factory;
  packet_loop_runner_opts.event_writer_factory = options_.event_writer_factory;
  packet_loop_runner_opts.packet_receiver_options.channel.event_id =
      event_update.event();
  packet_loop_runner_opts.current_event = event_update;
  packet_loop_runner_opts.event_commit_callback = [this](int64_t offset) {
    return CommitEventOffset(offset);
  };
  packet_loop_runner_ =
      std::make_unique<PacketLoopRunner>(packet_loop_runner_opts);
  t_ = std::thread([=]() { packet_loop_runner_->Run().IgnoreError(); });
}

void EventLoopRunner::Finalize() {
  if (packet_loop_runner_) {
    packet_loop_runner_->Cancel();
    t_.join();
    packet_loop_runner_.reset();
  }
  event_update_receiver_.reset();
}

void EventLoopRunner::Cancel() { is_canceled_.Notify(); }

void EventLoopRunner::CommitEventOffset(int64_t offset) {
  bool commit_ok;
  {
    absl::MutexLock lock(&local_commit_offset_mu_);
    local_commit_offset_ = offset;
  }
  if (!event_update_receiver_) {
    LOG(WARNING) << "EventUpdateReceiver unitialized";
    return;
  }
  if (!event_update_receiver_->Commit(kCommitEventTimeout, offset,
                                      &commit_ok)) {
    LOG(ERROR) << "Event Commit Timeout";
    return;
  }
  if (!commit_ok) {
    LOG(ERROR) << "Event Commit Failed";
  }
}

absl::Status EventLoopRunner::Run() {
  EventUpdate event_update;
  int64_t current_offset;
  while (!is_canceled_.HasBeenNotified()) {
    if (event_update_receiver_ == nullptr) {
      LOG(INFO) << "Initializing event update receiver";
      VAI_ASSIGN_OR_RETURN(
          event_update_receiver_,
          options_.event_receiver_factory(options_.event_receiver_options));
    }
    bool read_ok;
    if (!event_update_receiver_->Receive(kReceiveEventInterval, &event_update,
                                         &read_ok)) {
      continue;
    }
    if (!read_ok) {
      event_update_receiver_->CommitsDone();
      absl::Status s = event_update_receiver_->Finish();
      if (absl::IsOutOfRange(s)) {
        break;
      }
      LOG(INFO) << "EventUpdateReceiver finished with status " << s
                << ". Restarting.";
      event_update_receiver_.reset();
      continue;
    }
    current_offset = GetOffset(event_update);
    {
      absl::MutexLock lock(&local_commit_offset_mu_);
      if (current_offset <= local_commit_offset_) {
        LOG(INFO) << "Skipping event " << event_update.event();
        continue;
      }
    }
    OnReceiveEvent(event_update);
  }
  Finalize();
  return absl::OkStatus();
}

}  // namespace visionai