/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_UTIL_PROTO_UTIL_H_
#define THIRD_PARTY_VISIONAI_UTIL_PROTO_UTIL_H_

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "google/protobuf/text_format.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

using ::google::cloud::visionai::v1::StreamAnnotationType;

// Reads and parses a protobuf from a file.
template <class Proto>
absl::StatusOr<Proto> ReadAndParseProtoFromFile(std::string proto_path) {
  std::string proto_contents;
  VAI_RETURN_IF_ERROR(GetFileContents(proto_path, &proto_contents));
  Proto proto;

  if (!google::protobuf::TextFormat::ParseFromString(proto_contents, &proto)) {
    return absl::InternalError("Cannot parse the graph.");
  }

  return proto;
}

// Sets the zone or line annotation string from serialized proto string.
//
// If is_web_base64_string is true, it is expect that the string is encoded with
// absl::WebSafeBase64Escape
absl::StatusOr<std::string> SetAnnotations(std::string serialized_annotation,
                                           bool is_web_base64_string = true);

// Build StreamAnnotations for zones and lines and serialize the proto to
// string.
//
// If use_web_base64_string is true, it will encode the string with
// absl::WebSafeBase64Escape.
absl::StatusOr<std::string> BuildSerializedStreamAnnotations(
    std::string polylines_string, StreamAnnotationType annotation_type,
    bool use_web_base64_string = true);

}  // namespace visionai

#endif  //  THIRD_PARTY_VISIONAI_UTIL_PROTO_UTIL_H_
