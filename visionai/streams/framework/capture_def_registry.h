/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_DEF_REGISTRY_H_
#define THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_DEF_REGISTRY_H_

#include <functional>
#include <string>

#include "absl/container/flat_hash_map.h"
#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"
#include "visionai/streams/framework/capture_def.pb.h"
#include "visionai/streams/framework/capture_def_builder.h"
#include "visionai/streams/framework/registration.h"

namespace visionai {

// CaptureDefRegistry is meant to be used as a global static registry that
// contains the set of all CaptureDefs that have been defined by source capture
// contributors.
//
// It is currently NOT thread safe.
class CaptureDefRegistry {
 public:
  typedef std::function<absl::Status(CaptureDef*)> CaptureDefFactory;

  CaptureDefRegistry();

  // Get a pointer to the global singleton registry.
  static CaptureDefRegistry* Global();

  // Register the CaptureDef that built by the given `capture_def_factory`.
  //
  // Note that it is the CaptureDef that is stored in the registry, and the
  // CaptureDefFactory is simply used to generate it; i.e. we do not register
  // CaptureDef's directly, only indirectory through a factory.
  //
  // CHECK fails if the registration was unsuccessful.
  void Register(const CaptureDefFactory& capture_def_factory);

  // Lookup the CaptureDef corresponding to the specific source capture
  // `capture_name`.
  absl::StatusOr<const CaptureDef*> LookUp(absl::string_view capture_name);

 private:
  absl::Status RegisterHelper(const CaptureDefFactory& capture_def_factory);

  absl::flat_hash_map<std::string, const CaptureDef*> registry_;
};

namespace internal_registration {

class CaptureDefBuilderWrapper {
 public:
  explicit CaptureDefBuilderWrapper(absl::string_view name) : builder_(name) {}

  CaptureDefBuilderWrapper& OutputPacketType(
      absl::string_view output_packet_type) {
    builder_.OutputPacketType(output_packet_type);
    return *this;
  }

  CaptureDefBuilderWrapper& Attr(absl::string_view name,
                                 absl::string_view type) {
    builder_.Attr(name, type);
    return *this;
  }

  CaptureDefBuilderWrapper& Doc(absl::string_view doc) {
    builder_.Doc(doc);
    return *this;
  }

  InitOnStartupMarker operator()();

 private:
  CaptureDefBuilder builder_;
};

}  // namespace internal_registration

// To register a capture declaration, use a macro similar to the example below:
//
// REGISTER_CAPTURE_INTERFACE("MyCapture")
//   .OutputPacketType("..some packet type..")
//   .Attr("my_int_attr", "int")
//   .Attr("my_float_attr", "float")
//   .Attr("my_string_attr", "string")
//   .Doc("Some documentation.");
//
#define REGISTER_CAPTURE_INTERFACE_IMPL(ctr, name)                         \
  static ::visionai::InitOnStartupMarker const register_capture_def##ctr = \
      ::visionai::InitOnStartupMarker{}                                    \
      << ::visionai::internal_registration::CaptureDefBuilderWrapper(name)

#define REGISTER_CAPTURE_INTERFACE(name) \
  VAI_NEW_ID_FOR_INIT(REGISTER_CAPTURE_INTERFACE_IMPL, name)

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_CAPTURE_DEF_REGISTRY_H_
