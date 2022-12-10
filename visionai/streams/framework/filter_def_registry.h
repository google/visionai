/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_FILTER_DEF_REGISTRY_H_
#define THIRD_PARTY_VISIONAI_STREAMS_FILTER_DEF_REGISTRY_H_

#include <functional>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/filter_def.pb.h"
#include "visionai/streams/framework/filter_def_builder.h"
#include "visionai/streams/framework/registration.h"

namespace visionai {

// FilterDefRegistry is meant to be used as a global static registry that
// contains the set of all FilterDefs that have been defined by source filter
// contributors.
//
// It is currently NOT thread safe.
class FilterDefRegistry {
 public:
  typedef std::function<absl::Status(FilterDef*)> FilterDefFactory;

  FilterDefRegistry();

  // Get a pointer to the global singleton registry.
  static FilterDefRegistry* Global();

  // Register the FilterDef that is built by the given `filter_def_factory`.
  //
  // Note that it is the FilterDef that is stored in the registry, and the
  // FilterDefFactory is simply used to generate it; i.e. we do not register
  // FilterDef's directly, only indirectly through the factory.
  //
  // CHECK fails if the registration was unsuccessful.
  void Register(const FilterDefFactory& filter_def_factory);

  // Lookup the FilterDef corresponding to the specific source filter
  // `filter_name`.
  absl::StatusOr<const FilterDef*> LookUp(absl::string_view filter_name);

 private:
  absl::Status RegisterHelper(const FilterDefFactory& filter_def_factory);

  absl::flat_hash_map<std::string, const FilterDef*> registry_;
};

namespace internal_registration {

class FilterDefBuilderWrapper {
 public:
  explicit FilterDefBuilderWrapper(absl::string_view name) : builder_(name) {}

  FilterDefBuilderWrapper& InputPacketType(
      absl::string_view input_packet_type) {
    builder_.InputPacketType(input_packet_type);
    return *this;
  }

  FilterDefBuilderWrapper& OutputPacketType(
      absl::string_view output_packet_type) {
    builder_.OutputPacketType(output_packet_type);
    return *this;
  }

  FilterDefBuilderWrapper& Attr(absl::string_view name,
                                absl::string_view type) {
    builder_.Attr(name, type);
    return *this;
  }

  FilterDefBuilderWrapper& Doc(absl::string_view doc) {
    builder_.Doc(doc);
    return *this;
  }

  InitOnStartupMarker operator()();

 private:
  FilterDefBuilder builder_;
};

}  // namespace internal_registration

// To register a filter declaration, use a macro similar to the example below:
//
// REGISTER_FILTER_INTERFACE("MyFilter")
//   .InputPacketType("..some packet type..")
//   .OutputPacketType("..some packet type..")
//   .Attr("my_int_attr", "int")
//   .Attr("my_float_attr", "float")
//   .Attr("my_string_attr", "string")
//   .Doc("Some documentation.");
//
#define REGISTER_FILTER_INTERFACE_IMPL(ctr, name)                         \
  static ::visionai::InitOnStartupMarker const register_filter_def##ctr = \
      ::visionai::InitOnStartupMarker{}                                   \
      << ::visionai::internal_registration::FilterDefBuilderWrapper(name)

#define REGISTER_FILTER_INTERFACE(name) \
  VAI_NEW_ID_FOR_INIT(REGISTER_FILTER_INTERFACE_IMPL, name)

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_FILTER_DEF_REGISTRY_H_
