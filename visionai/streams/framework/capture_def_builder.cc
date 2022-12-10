// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/framework/capture_def_builder.h"

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/attr_def.pb.h"

namespace visionai {

CaptureDefBuilder::CaptureDefBuilder(absl::string_view name) {
  capture_def_.set_name(std::string(name));
}

CaptureDefBuilder& CaptureDefBuilder::OutputPacketType(
    absl::string_view output_packet_type) {
  capture_def_.set_output_packet_type(std::string(output_packet_type));
  return *this;
}

CaptureDefBuilder& CaptureDefBuilder::Attr(absl::string_view name,
                                           absl::string_view type) {
  auto attr_def = capture_def_.add_attr();
  attr_def->set_name(std::string(name));
  attr_def->set_type(std::string(type));
  return *this;
}

CaptureDefBuilder& CaptureDefBuilder::Doc(absl::string_view doc) {
  capture_def_.set_doc(std::string(doc));
  return *this;
}

absl::Status CaptureDefBuilder::Finalize(CaptureDef* capture_def) const {
  *capture_def = capture_def_;
  return absl::OkStatus();
}

}  // namespace visionai
