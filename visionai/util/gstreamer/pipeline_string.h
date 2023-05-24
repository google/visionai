// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_UTIL_GSTREAMER_PIPELINE_STRING_H_
#define THIRD_PARTY_VISIONAI_UTIL_GSTREAMER_PIPELINE_STRING_H_

#include <string>

#include "absl/status/statusor.h"
#include "absl/strings/string_view.h"

namespace visionai {

std::string FileSrcGstPipelineStr(absl::string_view input_file_path);

absl::StatusOr<std::string> Mp4FileSinkH264GstPipelineStr(
    absl::string_view output_file_path);

absl::StatusOr<std::string> Mp4FileSinkTranscodeGstPipelineStr(
    std::string caps_string,
    absl::string_view output_file_path);

absl::StatusOr<std::string> GetFramerateFractionFromCaps(
  const std::string& caps_string);

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_UTIL_GSTREAMER_PIPELINE_STRING_H_
