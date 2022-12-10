// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd
//
// -----------------------------------------------------------------------------
// event_update.h
// -----------------------------------------------------------------------------
//
// This header defines how `google.cloud.visionai.v1alpha1.EventUpdate`s are
// used throughout C++ code.

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CLIENT_EVENT_UPDATE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CLIENT_EVENT_UPDATE_H_

#include <cstdint>
#include <utility>

#include "google/cloud/visionai/v1/streaming_service.pb.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/time/clock.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

// ----------------------------------------------------------------------------
// Type Aliases
// ----------------------------------------------------------------------------

using EventUpdate = google::cloud::visionai::v1::EventUpdate;

// ----------------------------------------------------------------------------
// EventUpdate Accessors and Mutators
// ----------------------------------------------------------------------------
//
// The methods below are functions that access or mutate `EventUpdate`s.
// Please use these methods instead of directly doing so through the
// `EventUpdate` message, especially for mutators.

// Get the offset from `event_update`.
int64_t GetOffset(const EventUpdate& event_update);

// Set the offset for `event_update`.
absl::Status SetOffset(int64_t offset, EventUpdate* event_update);

// Get the stream id from `event_update`.
absl::StatusOr<std::string> GetStreamId(const EventUpdate& event_update);

// Get the event id from `event_update`.
absl::StatusOr<std::string> GetEventId(const EventUpdate& event_update);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CLIENT_EVENT_UPDATE_H_
