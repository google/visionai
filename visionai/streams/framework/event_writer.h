/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_H_

#include <memory>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "absl/time/time.h"
#include "visionai/proto/cluster_selection.pb.h"
#include "visionai/streams/framework/attr_value.pb.h"
#include "visionai/streams/framework/attr_value_util.h"
#include "visionai/streams/framework/registration.h"
#include "visionai/proto/ingester_config.pb.h"
#include "visionai/streams/packet/packet.h"
#include "visionai/util/ring_buffer.h"

namespace visionai {

// EventWriterInitContext is the object passed to the `Initialize` routine to
// give the EventWriters context from which to initialize itself.
class EventWriterInitContext {
 public:
  // Create an EventWriterInitContext from `config`.
  static absl::StatusOr<std::unique_ptr<EventWriterInitContext>> Create(
      const EventWriterConfig& config);

  // Get the value of a specific attribute.
  //
  // If the user supplied a value, then `out` will be overwritten. Otherwise,
  // leave the value of `out` as is. That is, you may initialize `out` to a
  // default value before calling this function.
  template <typename T>
  absl::Status GetAttr(absl::string_view name, T* out) const;

  // Get the cluster that has been selected.
  //
  // TODO: Consider supplying higher level methods that will more directly give
  // access (and checks) to the member data.
  absl::StatusOr<ClusterSelection> GetClusterSelection() const;

  EventWriterInitContext() = default;

 private:
  absl::flat_hash_map<std::string, AttrValue> attrs_;
  ClusterSelection cluster_selection_;

  absl::Status Initialize(const EventWriterConfig& config);
};

// EventWriter is the base class for all event writers.
// Derive from it to write your specific event writer.
//
// Example:
// class MyEventWriter : public EventWriter {
//   // fill in the blanks.
// };
class EventWriter {
 public:
  virtual ~EventWriter() = default;

  // Initializes the EventWriter.
  //
  // You may perform expensive initializations as this is done upfront.
  virtual absl::Status Init(EventWriterInitContext* ctx) = 0;

  // Method to create the objects that events get mapped to, and prepare it into
  // a state that is ready to accept writes.
  //
  // Examples:
  //   + Streams: Create events, materialize channels, etc.
  //   + Files: Create the file itself and open it for writing.
  virtual absl::Status Open(absl::string_view event_id) = 0;

  // Method to write a single packet into the object.
  //
  // TODO: Change to Packet when we have it ported from GoB.
  //
  // Examples:
  //   + Streams: Send the packet.
  //   + Files: Write an element into the file.
  virtual absl::Status Write(Packet) = 0;

  // Method to close the object.
  //
  // Examples:
  //   + Streams: Release the writer's lease.
  //   + Files: Close the file.
  virtual absl::Status Close() = 0;
};

// EventWriterRegistry is a static global registry that contains functors for
// instantiating instances of specific event writers.
//
// It is currently NOT thread safe.
class EventWriterRegistry {
 public:
  typedef std::function<EventWriter*(void)> EventWriterFactory;

  EventWriterRegistry() = default;

  // Get an instance of the global registry.
  static EventWriterRegistry* Global();

  // Register the EventWriterFactory to the given `event_writer_name`.
  //
  // Note that we are registering the factory itself. The factory will be called
  // to instantiate the actual EventWriter if it is actually requested at
  // runtime.
  void Register(absl::string_view event_writer_name,
                EventWriterFactory event_writer_factory);

  // Create a EventWriter registered against the given `event_writer_name`.
  absl::StatusOr<std::unique_ptr<EventWriter>> CreateEventWriter(
      absl::string_view event_writer_name);

 private:
  absl::Status RegisterHelper(absl::string_view event_writer_name,
                              EventWriterFactory event_writer_factory);

  absl::flat_hash_map<std::string, EventWriterFactory> registry_;
};

// To register a new EventWriter definition:
//
// // Assuming you implemented MyEventWriter:
// class MyEventWriter : public EventWriter {
//   // ...
// };
//
// // Register the class against the corresponding EventWriterDef name.
// REGISTER_EVENT_WRITER_DEFINITION("MyEventWriter", MyEventWriter);
#define REGISTER_EVENT_WRITER_IMPLEMENTATION_IMPL(ctr, name, ...)           \
  static ::visionai::InitOnStartupMarker const register_event_writer##ctr = \
      ::visionai::InitOnStartupMarker{} << ([]() {                          \
        ::visionai::EventWriterRegistry::Global()->Register(                \
            name,                                                           \
            []() -> ::visionai::EventWriter* { return new __VA_ARGS__; });  \
        return ::visionai::InitOnStartupMarker{};                           \
      })

#define REGISTER_EVENT_WRITER_IMPLEMENTATION(name, ...)                \
  VAI_NEW_ID_FOR_INIT(REGISTER_EVENT_WRITER_IMPLEMENTATION_IMPL, name, \
                      __VA_ARGS__)

// -----------------------------------------------------------------------------
// Implementation below; please ignore.

template <typename T>
absl::Status EventWriterInitContext::GetAttr(absl::string_view name,
                                             T* out) const {
  auto it = attrs_.find(name);
  if (it == attrs_.end()) {
    return absl::OkStatus();
  }
  auto value = GetAttrValue<T>(it->second);
  if (!value.ok()) {
    return value.status();
  }
  *out = *value;
  return absl::OkStatus();
}

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_H_
