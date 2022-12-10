/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_FILTER_DEF_BUILDER_H_
#define THIRD_PARTY_VISIONAI_STREAMS_FILTER_DEF_BUILDER_H_

#include <string>

#include "absl/status/status.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/filter_def.pb.h"

namespace visionai {

// FilterDefBuilder is a builder class to build a FilterDef message.
//
// It is primarily useful for supporting the registration syntax.
class FilterDefBuilder {
 public:
  // Construct a builder that will build a FilterDef with name `name`.
  explicit FilterDefBuilder(absl::string_view name);
  ~FilterDefBuilder() = default;

  // Decides the input type of the packets that this source filter will
  // consume. It must be the stringified representation.
  //
  // TODO: Add the location at which the stringfied representation is
  // documented. Probably after Packet is ported from GoB.
  FilterDefBuilder& InputPacketType(absl::string_view input_packet_type);

  // Decides the output type of the packets that this source filter will
  // produce. It must be the stringified representation.
  //
  // TODO: Add the location at which the stringfied representation is
  // documented. Probably after Packet is ported from GoB.
  FilterDefBuilder& OutputPacketType(absl::string_view output_packet_type);

  // Attach an attribute of name `name` accepting values of type `type`.
  //
  // See attr_def.proto for what types are possible.
  FilterDefBuilder& Attr(absl::string_view name, absl::string_view type);

  // Add documenation about this source filter.
  FilterDefBuilder& Doc(absl::string_view doc);

  // Builds the FilterDef message and saves it to `filter_def`.
  absl::Status Finalize(FilterDef* filter_def) const;

 private:
  FilterDef filter_def_;
};

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_FILTER_DEF_BUILDER_H_
