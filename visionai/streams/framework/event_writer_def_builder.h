/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_DEF_BUILDER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_DEF_BUILDER_H_

#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/event_writer_def.pb.h"

namespace visionai {

// EventWriterDefBuilder is a builder class to build a EventWriterDef message.
//
// It is primarily useful for supporting the registration syntax.
class EventWriterDefBuilder {
 public:
  // Construct a builder that will build a EventWriterDef with name `name`.
  explicit EventWriterDefBuilder(absl::string_view name);
  ~EventWriterDefBuilder() = default;

  // Decides the input type of the packets that this event writer will
  // consume. It must be the stringified representation.
  //
  // TODO: Add the location at which the stringfied representation is
  // documented. Probably after Packet is ported from GoB.
  EventWriterDefBuilder& InputPacketType(absl::string_view input_packet_type);

  // Attach an attribute of name `name` accepting values of type `type`.
  //
  // See attr_def.proto for what types are possible.
  EventWriterDefBuilder& Attr(absl::string_view name, absl::string_view type);

  // Add documenation about this event writer.
  EventWriterDefBuilder& Doc(absl::string_view doc);

  // Builds the EventWriterDef message and saves it to `event_writer_def`.
  absl::Status Finalize(EventWriterDef* event_writer_def) const;

 private:
  EventWriterDef event_writer_def_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_EVENT_WRITER_DEF_BUILDER_H_
