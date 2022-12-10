// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_UTIL_EVENT_LOOP_RUNNER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_UTIL_EVENT_LOOP_RUNNER_H_

#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "absl/log/log.h"
#include "absl/status/status.h"
#include "absl/synchronization/notification.h"
#include "visionai/streams/apps/util/packet_loop_runner.h"
#include "visionai/streams/client/event_update.h"
#include "visionai/streams/client/event_update_receiver.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

using EventReceiverFactory =
    std::function<absl::StatusOr<std::shared_ptr<EventUpdateReceiver>>(
        const EventUpdateReceiver::Options& options)>;

// `EventLoopRunner` repetitively reads events from `EventUpdateReceiver` and
// invokes `PacketLoopRunner` to process packets in each event.
//
// Note: `EventLoopRunner` operates on a sequential mode, processing one event
// at a time. Whenever a new event arrives, the `PacketLoopRunner` of the last
// event will be canceled.
//
// --------------------------
// Receiving Events
// --------------------------
// The `EventLoopRunner` creates an instance of `EventUpdateReceiver` with the
// given factory method and the option. Some possible behaviours and outcomes
// while receiving the events:
//
// (1) Success:
//      Cancel the `PacketLoopRunner` of the previous event (if it exists) and
//      launch the new pipeline for the current event.
// (2) Timeout:
//      Continue to poll the events with the same `EventUpdateReceiver`.
//      instance.
// (3) Finish with `absl::OutOfRange status`:
//      No more events is coming. `EventLoopRunner::Run()` returns OK.
// (4) Finish with other error status:
//      Create another `EventLoopRunner` instance and retry.
//
// --------------------------
// Committing Events
// --------------------------
// The function `EventLoopRunner::CommitEventOffset()` is passed to the
// `PacketLoopRunner`, and the `PacketLoopRunner` can decide when to invoke this
// callback to commit a particular event offset.
//
// The event commit is conducted as a fire-and-forget operation. We only
// log the error encountered in the commit process.
//
// If the packet commit failed because the underlying gRPC stream terminated,
// the main runner can identify the issue with `EventUpdateReceiver::Receive()`
// and retry.
//
// The `EventLoopRunner` also keeps track of the `local_commit_offset_` to
// deduplicate the events in the case of commit failures.
class EventLoopRunner {
 public:
  struct Options {
    // The factory to create the `EventUpdateReceiver` instances.
    EventReceiverFactory event_receiver_factory;

    // The options to create the `EventUpdateReceiver` instances.
    EventUpdateReceiver::Options event_receiver_options;

    // The factory to create the `PacketReceiver` instances.
    // Note: Only the controlled mode is accepted.
    PacketReceiverFactory packet_receiver_factory;

    // The options to create the `PacketReceiver` instances.
    // Note: The field `channel.event_id` shall be left empty and filled by
    // EventLoopRunner.
    PacketReceiver::Options packet_receiver_options;

    // The factory to create the `EventWriter` instance.
    EventWriterFactory event_writer_factory;
  };

  EventLoopRunner(const Options& options)
      : options_(options), is_canceled_(false) {}

  absl::Status Run();

  // Cancel the runner.
  void Cancel();

 private:
  Options options_;
  absl::Notification is_canceled_;
  absl::Mutex local_commit_offset_mu_;
  int64_t local_commit_offset_ = -1;
  std::shared_ptr<EventUpdateReceiver> event_update_receiver_ = nullptr;
  std::shared_ptr<PacketLoopRunner> packet_loop_runner_ = nullptr;
  std::thread t_;

  void CommitEventOffset(int64_t offset);
  void OnReceiveEvent(EventUpdate event_update);
  void Finalize();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_UTIL_EVENT_LOOP_RUNNER_H_
