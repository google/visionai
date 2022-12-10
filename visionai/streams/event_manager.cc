// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/event_manager.h"

#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/streams/event_sink.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/util/random_string.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
constexpr size_t kDefaultEventNameLength = 8;
}  // namespace

EventManager::EventManager(const Options& options) : options_(options) {
  event_ = options_.ingest_policy.event();
  event_idx_.store(0);
}

absl::Status EventManager::CreateAndInsertNewEventSink(
    const std::string& event_id) {
  EventSink::Options options;
  options.event_id = event_id;
  options.event_writer_config = options_.config;
  VAI_ASSIGN_OR_RETURN(auto event_sink, EventSink::Create(options),
                   _ << "while creating an EventSink");
  {
    absl::MutexLock lock(&sinks_mu_);
    auto p = sinks_.emplace(std::make_pair(event_id, event_sink));
    if (!p.second) {
      return absl::InternalError(absl::StrFormat(
          "Failed to insert a new EventSink due to duplicated key \"%s\".",
          event_id));
    }
  }
  return absl::OkStatus();
}

absl::StatusOr<std::string> EventManager::OpenForContinuousMode() {
  // For continuous mode, only a single event sink is ever used.
  // Return immediately if it is already present.
  {
    absl::MutexLock lock(&sinks_mu_);
    if (!sinks_.empty()) {
      return sinks_.begin()->first;
    }
  }

  // If this is the first time encountering an event, create the event sink.
  if (event_.empty()) {
    VAI_ASSIGN_OR_RETURN(event_, RandomResourceId(kDefaultEventNameLength));
    LOG(INFO) << absl::StrFormat("Generated event-id: %s", event_);
  }
  VAI_RETURN_IF_ERROR(CreateAndInsertNewEventSink(event_));

  return event_;
}

absl::StatusOr<std::string> EventManager::OpenForSequentialMode() {
  // For sequential mode, every request to open will generate a new
  // event. Simply use a different suffix.
  if (event_.empty()) {
    VAI_ASSIGN_OR_RETURN(event_, RandomResourceId(kDefaultEventNameLength));
    LOG(INFO) << absl::StrFormat("Generated event prefix: %s", event_);
  }
  std::string event_id =
      absl::StrFormat("%s-%d", event_, event_idx_.fetch_add(1));
  VAI_RETURN_IF_ERROR(CreateAndInsertNewEventSink(event_id));
  return event_id;
}

absl::StatusOr<std::string> EventManager::Open() {
  if (options_.ingest_policy.continuous_mode()) {
    return OpenForContinuousMode();
  } else {
    return OpenForSequentialMode();
  }
}

absl::StatusOr<std::shared_ptr<EventSink>> EventManager::GetEventSink(
    absl::string_view event_id) {
  absl::MutexLock lock(&sinks_mu_);
  auto it = sinks_.find(event_id);
  if (it == sinks_.end()) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot find event id \"%s\".", event_id));
  }
  return it->second;
}

absl::Status EventManager::Close(absl::string_view event_id) {
  absl::MutexLock lock(&sinks_mu_);
  auto it = sinks_.find(event_id);
  if (it == sinks_.end()) {
    return absl::InvalidArgumentError(
        absl::StrFormat("Cannot find event id \"%s\".", event_id));
  }

  // Only remove the event sink for sequential mode.
  // Keep the same event sink around for continuous mode.
  if (!options_.ingest_policy.continuous_mode()) {
    it->second->Close();
    sinks_.erase(it);
  }
  return absl::OkStatus();
}

}  // namespace visionai
