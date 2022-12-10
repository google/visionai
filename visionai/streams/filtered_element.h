// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_FILTERED_ELEMENT_H_
#define THIRD_PARTY_VISIONAI_STREAMS_FILTERED_ELEMENT_H_

#include <memory>
#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/packet/packet.h"

namespace visionai {

// Types of a `FilteredElement`.
enum class FilteredElementType { kOpen, kPacket, kClose };

// A `FilteredElement` are datum produced by the filter module and consumed by
// the depositor for de-multiplexing into different event destination.
//
// They may be exactly one of the following:
//
// + An data element that contains a `Packet` meant for a specific event.
// + An control element that signals that a specific event is open.
// + An control element that signals that a specific event should be closed.
//
// You should use one of the free functions to use these elements rather than
// directly constructing and mutating the FilterElement objects directly.
class FilteredElement {
 public:
  // Create a default FilteredElement.
  explicit FilteredElement() = default;

  // Set the type.
  void set_type(FilteredElementType type) { type_ = type; }

  // Get the type.
  FilteredElementType type() const { return type_; }

  // Set the event id.
  void set_event_id(absl::string_view event_id) {
    event_id_ = std::string(event_id);
  }

  // Get the event id.
  const std::string& event_id() const { return event_id_; }

  // Transfer the `Packet` in.
  void SetPacket(Packet p) { packet_ = std::make_unique<Packet>(std::move(p)); }

  // Release the held `Packet`.
  std::unique_ptr<Packet> ReleasePacket() && { return std::move(packet_); }

  ~FilteredElement() = default;
  FilteredElement(FilteredElement&&) = default;
  FilteredElement& operator=(FilteredElement&&) = default;

 private:
  FilteredElement(const FilteredElement&) = delete;
  FilteredElement& operator=(const FilteredElement&) = delete;

  FilteredElementType type_;
  std::string event_id_;
  std::unique_ptr<Packet> packet_ = nullptr;
};

// Create a FilteredElement intended for `event_id`.
//
// The first overload creates a `FilteredElement` of type `kPacket` containing
// `packet`.
// The second overload creates a `FilteredElement` of type `kOpen`.
// The third overload creates a `FilteredElement` of type `kClose`.
absl::StatusOr<FilteredElement> MakePacketFilteredElement(
    absl::string_view event_id, Packet packet);
absl::StatusOr<FilteredElement> MakeOpenFilteredElement(
    absl::string_view event_id);
absl::StatusOr<FilteredElement> MakeCloseFilteredElement(
    absl::string_view event_id);

// Extract the packet from the given filtered element.
absl::StatusOr<Packet> PacketFromFilteredElement(FilteredElement f);

// Decide whether the given `FilteredElement` is `kOpen`.
bool IsOpenType(const FilteredElement&);

// Decide whether the given `FilteredElement` is `kClose`.
bool IsCloseType(const FilteredElement&);

// Decide whether the given `FilteredElement` is `kPacket`.
bool IsPacketType(const FilteredElement&);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_FILTERED_ELEMENT_H_
