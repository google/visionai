/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_DEF_BUILDER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_DEF_BUILDER_H_

#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/capture_def.pb.h"

namespace visionai {

// CaptureDefBuilder is a builder class to build a CaptureDef message.
//
// It is primarily useful for supporting the registration syntax.
class CaptureDefBuilder {
 public:
  // Construct a builder that will build a CaptureDef with name `name`.
  explicit CaptureDefBuilder(absl::string_view name);
  ~CaptureDefBuilder() = default;

  // Decides the output type of the packets that this source capture will
  // produce. It must be the stringified representation.
  //
  // TODO: Add the location at which the stringfied representation is
  // documented. Probably after Packet is ported from GoB.
  CaptureDefBuilder& OutputPacketType(absl::string_view output_packet_type);

  // Attach an attribute of name `name` accepting values of type `type`.
  //
  // See capture_def.proto for what types are possible.
  CaptureDefBuilder& Attr(absl::string_view name, absl::string_view type);

  // Add documenation about this source capture.
  CaptureDefBuilder& Doc(absl::string_view doc);

  // Builds the CaptureDef message and saves it to `capture_def`.
  absl::Status Finalize(CaptureDef* capture_def) const;

 private:
  CaptureDef capture_def_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_DEF_BUILDER_H_
