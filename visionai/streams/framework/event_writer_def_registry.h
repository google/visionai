/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_DEF_REGISTRY_H_
#define THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_DEF_REGISTRY_H_

#include <functional>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/event_writer_def.pb.h"
#include "visionai/streams/framework/event_writer_def_builder.h"
#include "visionai/streams/framework/registration.h"

namespace visionai {

// EventWriterDefRegistry is meant to be used as a global static registry that
// contains the set of all EventWriterDefs that have been defined by event
// writer contributors.
//
// It is currently NOT thread safe.
class EventWriterDefRegistry {
 public:
  typedef std::function<absl::Status(EventWriterDef*)> EventWriterDefFactory;

  EventWriterDefRegistry();

  // Get a pointer to the global singleton registry.
  static EventWriterDefRegistry* Global();

  // Register the EventWriterDef that is built by the given
  // `event_writer_def_factory`.
  //
  // Note that it is the EventWriterDef that is stored in the registry, and the
  // EventWriterDefFactory is simply used to generate it; i.e. we do not
  // register EventWriterDef's directly, only indirectly through the factory.
  //
  // CHECK fails if the registration was unsuccessful.
  void Register(const EventWriterDefFactory& event_writer_def_factory);

  // Lookup the EventWriterDef corresponding to the specific event writer
  // `event_writer_name`.
  absl::StatusOr<const EventWriterDef*> LookUp(
      absl::string_view event_writer_name);

 private:
  absl::Status RegisterHelper(
      const EventWriterDefFactory& event_writer_def_factory);

  absl::flat_hash_map<std::string, const EventWriterDef*> registry_;
};

namespace internal_registration {

class EventWriterDefBuilderWrapper {
 public:
  explicit EventWriterDefBuilderWrapper(absl::string_view name)
      : builder_(name) {}

  EventWriterDefBuilderWrapper& InputPacketType(
      absl::string_view input_packet_type) {
    builder_.InputPacketType(input_packet_type);
    return *this;
  }

  EventWriterDefBuilderWrapper& Attr(absl::string_view name,
                                     absl::string_view type) {
    builder_.Attr(name, type);
    return *this;
  }

  EventWriterDefBuilderWrapper& Doc(absl::string_view doc) {
    builder_.Doc(doc);
    return *this;
  }

  InitOnStartupMarker operator()();

 private:
  EventWriterDefBuilder builder_;
};

}  // namespace internal_registration

// To register a event_writer declaration, use a macro similar to the example
// below:
//
// REGISTER_EVENT_WRITER_INTERFACE("MyEventWriter")
//   .InputPacketType("..some packet type..")
//   .Attr("my_int_attr", "int")
//   .Attr("my_float_attr", "float")
//   .Attr("my_string_attr", "string")
//   .Doc("Some documentation.");
//
#define REGISTER_EVENT_WRITER_INTERFACE_IMPL(ctr, name)                       \
  static ::visionai::InitOnStartupMarker const                                \
      register_event_writer_def##ctr =                                        \
          ::visionai::InitOnStartupMarker{}                                   \
          << ::visionai::internal_registration::EventWriterDefBuilderWrapper( \
                 name)

#define REGISTER_EVENT_WRITER_INTERFACE(name) \
  VAI_NEW_ID_FOR_INIT(REGISTER_EVENT_WRITER_INTERFACE_IMPL, name)

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_DEF_REGISTRY_H_
