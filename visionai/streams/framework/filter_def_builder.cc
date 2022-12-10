// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/filter_def_builder.h"

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/attr_def.pb.h"

namespace visionai {

FilterDefBuilder::FilterDefBuilder(absl::string_view name) {
  filter_def_.set_name(std::string(name));
}

FilterDefBuilder& FilterDefBuilder::InputPacketType(
    absl::string_view input_packet_type) {
  filter_def_.set_input_packet_type(std::string(input_packet_type));
  return *this;
}

FilterDefBuilder& FilterDefBuilder::OutputPacketType(
    absl::string_view output_packet_type) {
  filter_def_.set_output_packet_type(std::string(output_packet_type));
  return *this;
}

FilterDefBuilder& FilterDefBuilder::Attr(absl::string_view name,
                                         absl::string_view type) {
  auto attr_def = filter_def_.add_attr();
  attr_def->set_name(std::string(name));
  attr_def->set_type(std::string(type));
  return *this;
}

FilterDefBuilder& FilterDefBuilder::Doc(absl::string_view doc) {
  filter_def_.set_doc(std::string(doc));
  return *this;
}

absl::Status FilterDefBuilder::Finalize(FilterDef* filter_def) const {
  *filter_def = filter_def_;
  return absl::OkStatus();
}

}  // namespace visionai
