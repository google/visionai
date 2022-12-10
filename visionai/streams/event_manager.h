// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_EVENT_MANAGER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_EVENT_MANAGER_H_

#include <atomic>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/event_sink.h"
#include "visionai/proto/ingester_config.pb.h"

namespace visionai {

// The EventManager manages the lifetime of EventSinks throughout the ingester.
// It allows different entites to request to creation/termination of events, and
// exposes to them the write head into which they may write Packets.
//
// The typical use of the EventManager within the Ingester is as follows:
// 1. Open:
//    The filter context uses the EventManager to open an event for writing.
//
// 2. Steady-state:
//    The filter/depositor collaborate to write Packets into the
//    write head of specific events.
//
// 3. Close:
//    The depositor, once it receives the signal that an event has
//    reached the end, will use the EventManager to request its closure.
class EventManager {
 public:
  // Options for configuring the event manager.
  struct Options {
    // The config specifying what kind of EventWriter to use.
    EventWriterConfig config;

    // The ingest policy.
    IngesterConfig::IngestPolicy ingest_policy;
  };

  // Creates an EventManager that is ready for use.
  explicit EventManager(const Options &options);

  // Open an event for writing.
  //
  // On success, returns the event id handle to the caller. They may then use
  // this id to get the corresponding write head for writing, and later use it
  // to `Close`.
  absl::StatusOr<std::string> Open();

  // Get the EvenSink for a specific event.
  //
  // The returned object may be used to write Packets into the event.
  absl::StatusOr<std::shared_ptr<EventSink>> GetEventSink(
      absl::string_view event_id);

  // Close an event from writing.
  //
  // On success, signals that the event shall be closed for writing.
  absl::Status Close(absl::string_view event_id);

  // Copy-control. Please use Create to generate new instances of this class.
  ~EventManager() = default;
  EventManager(const EventManager &) = delete;
  EventManager &operator=(const EventManager &) = delete;

 private:
  const Options options_;

  std::string event_;
  std::atomic<int> event_idx_;

  absl::Mutex sinks_mu_;
  absl::flat_hash_map<std::string, std::shared_ptr<EventSink>> sinks_;

  absl::Status CreateAndInsertNewEventSink(const std::string &event_id);
  absl::StatusOr<std::string> OpenForContinuousMode();
  absl::StatusOr<std::string> OpenForSequentialMode();
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_EVENT_MANAGER_H_
