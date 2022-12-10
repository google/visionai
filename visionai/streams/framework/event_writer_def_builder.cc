// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/event_writer_def_builder.h"

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/attr_def.pb.h"

namespace visionai {

EventWriterDefBuilder::EventWriterDefBuilder(absl::string_view name) {
  event_writer_def_.set_name(std::string(name));
}

EventWriterDefBuilder& EventWriterDefBuilder::InputPacketType(
    absl::string_view input_packet_type) {
  event_writer_def_.set_input_packet_type(std::string(input_packet_type));
  return *this;
}

EventWriterDefBuilder& EventWriterDefBuilder::Attr(absl::string_view name,
                                                   absl::string_view type) {
  auto attr_def = event_writer_def_.add_attr();
  attr_def->set_name(std::string(name));
  attr_def->set_type(std::string(type));
  return *this;
}

EventWriterDefBuilder& EventWriterDefBuilder::Doc(absl::string_view doc) {
  event_writer_def_.set_doc(std::string(doc));
  return *this;
}

absl::Status EventWriterDefBuilder::Finalize(
    EventWriterDef* event_writer_def) const {
  *event_writer_def = event_writer_def_;
  return absl::OkStatus();
}

}  // namespace visionai
