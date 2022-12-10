// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/client/event_update.h"

#include <cstdint>
#include <utility>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/str_format.h"
#include "visionai/streams/client/resource_util.h"
#include "visionai/util/time_util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

int64_t GetOffset(const EventUpdate& event_update) {
  return event_update.offset();
}

absl::Status SetOffset(int64_t offset, EventUpdate* event_update) {
  if (event_update == nullptr) {
    return absl::InvalidArgumentError("Given a nullptr to an `EventUpdate`");
  }
  event_update->set_offset(offset);
  return absl::OkStatus();
}

absl::StatusOr<std::string> GetStreamId(const EventUpdate& event_update) {
  if (event_update.stream().empty()) {
    return absl::InvalidArgumentError(
        "The given `EventUpdate` does not contain a stream name");
  }
  VAI_ASSIGN_OR_RETURN(auto stream_id, ResourceId(event_update.stream()),
                   _ << "while attempting to parse a stream id from an "
                        "`EventUpdate`'s stream name");
  return stream_id;
}

absl::StatusOr<std::string> GetEventId(const EventUpdate& event_update) {
  if (event_update.event().empty()) {
    return absl::InvalidArgumentError(
        "The given `EventUpdate` does not contain a event name");
  }
  VAI_ASSIGN_OR_RETURN(auto event_id, ResourceId(event_update.event()),
                   _ << "while attempting to parse a event id from an "
                        "`EventUpdate`'s event name");
  return event_id;
}

}  // namespace visionai
