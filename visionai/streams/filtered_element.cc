// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/filtered_element.h"

#include "absl/status/status.h"

namespace visionai {

absl::StatusOr<FilteredElement> MakePacketFilteredElement(
    absl::string_view event_id, Packet packet) {
  FilteredElement f;
  f.set_type(FilteredElementType::kPacket);
  f.set_event_id(event_id);
  f.SetPacket(std::move(packet));
  return std::move(f);
}

absl::StatusOr<FilteredElement> MakeOpenFilteredElement(
    absl::string_view event_id) {
  FilteredElement f;
  f.set_type(FilteredElementType::kOpen);
  f.set_event_id(event_id);
  return std::move(f);
}

absl::StatusOr<FilteredElement> MakeCloseFilteredElement(
    absl::string_view event_id) {
  FilteredElement f;
  f.set_type(FilteredElementType::kClose);
  f.set_event_id(event_id);
  return std::move(f);
}

absl::StatusOr<Packet> PacketFromFilteredElement(FilteredElement f) {
  if (!IsPacketType(f)) {
    return absl::InvalidArgumentError("Given a non-packet filtered element.");
  }
  auto p = std::move(f).ReleasePacket();
  return std::move(*p);
}

bool IsOpenType(const FilteredElement& f) {
  return f.type() == FilteredElementType::kOpen;
}

bool IsCloseType(const FilteredElement& f) {
  return f.type() == FilteredElementType::kClose;
}

bool IsPacketType(const FilteredElement& f) {
  return f.type() == FilteredElementType::kPacket;
}

}  // namespace visionai
