// Copyright 2021 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/gstreamer/pipeline_string.h"

#include <string>
#include <vector>

#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstcaps.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/util/gstreamer/constants.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

std::string FileSrcGstPipelineStr(absl::string_view input_file_path) {
  std::vector<std::string> pipeline_elements;
  pipeline_elements.push_back(
      absl::StrFormat("filesrc location=%s", input_file_path));
  pipeline_elements.push_back("parsebin");
  return absl::StrJoin(pipeline_elements, " ! ");
}

absl::StatusOr<std::string> Mp4FileSinkH264GstPipelineStr(
    absl::string_view output_file_path) {
  std::vector<std::string> pipeline_elements;
  pipeline_elements.push_back("video/x-h264");
  pipeline_elements.push_back("mp4mux");
  pipeline_elements.push_back(
      absl::StrFormat("filesink location=%s", output_file_path));
  return absl::StrJoin(pipeline_elements, " ! ");
}

absl::StatusOr<std::string> Mp4FileSinkTranscodeGstPipelineStr(
    std::string caps_string, absl::string_view output_file_path) {
  std::vector<std::string> pipeline_elements;
  VAI_ASSIGN_OR_RETURN(std::string framerate,
                   GetFramerateFractionFromCaps(caps_string));
  pipeline_elements.push_back("decodebin");
  pipeline_elements.push_back("videoconvert");
  pipeline_elements.push_back("video/x-raw");
  pipeline_elements.push_back("videorate");
  pipeline_elements.push_back(
      absl::StrFormat("video/x-raw,framerate=%s", framerate));
  pipeline_elements.push_back("x264enc");
  pipeline_elements.push_back("mp4mux");
  pipeline_elements.push_back(
      absl::StrFormat("filesink location=%s", output_file_path));
  return absl::StrJoin(pipeline_elements, " ! ");
}

absl::StatusOr<std::string> GetFramerateFractionFromCaps(
    const std::string& caps_string) {
  VAI_RETURN_IF_ERROR(GstInit());

  GstCaps* caps = gst_caps_from_string(caps_string.c_str());
  GstStructure* structure = gst_caps_get_structure(caps, 0);
  int fr_numerator = 0, fr_denominator = 0;
  bool has_framerate = gst_structure_get_fraction(
      structure, kFramerateFieldName, &fr_numerator, &fr_denominator);
  gst_caps_unref(caps);

  if (!has_framerate || fr_numerator == 0 || fr_denominator == 0) {
    fr_numerator = kDefaultFramerateNumerator;
    fr_denominator = kDefaultFramerateDenominator;
  }
  return absl::StrFormat("%d/%d", fr_numerator, fr_denominator);
}

}  // namespace visionai
