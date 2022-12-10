/*
 * Copyright 2021 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#include "visionai/algorithms/media/gstreamer_video_writer.h"

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gststructure.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/algorithms/media/util/util.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {

constexpr char kFramerateFieldName[] = "framerate";
constexpr int kDefaultFramerateNumerator = 25;
constexpr int kDefaultFramerateDenominator = 1;

absl::Status ValidateOptions(const GstreamerVideoWriter::Options& options) {
  if (options.file_path.empty()) {
    return absl::InvalidArgumentError(
        "You must supply the name of the output video file");
  }

  if (options.caps_string.empty()) {
    return absl::InvalidArgumentError(
        "You must supply the expected caps string of the incoming gstreamer "
        "buffers");
  }
  return absl::OkStatus();
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

absl::StatusOr<std::string> AssembleTranscodeMuxPipeline(
    const GstreamerVideoWriter::Options& options, std::string framerate) {
  std::vector<std::string> pipeline_elements;
  pipeline_elements.push_back("decodebin");
  pipeline_elements.push_back("videoconvert");
  pipeline_elements.push_back("video/x-raw");
  pipeline_elements.push_back("videorate");
  pipeline_elements.push_back(
      absl::StrFormat("video/x-raw,framerate=%s", framerate));
  pipeline_elements.push_back("x264enc");
  pipeline_elements.push_back("mp4mux");
  pipeline_elements.push_back(
      absl::StrFormat("filesink location=%s", options.file_path));
  return absl::StrJoin(pipeline_elements, " ! ");
}

absl::StatusOr<std::string> AssembleH264MuxPipeline(
    const GstreamerVideoWriter::Options& options) {
  std::vector<std::string> pipeline_elements;
  pipeline_elements.push_back("video/x-h264");
  pipeline_elements.push_back("mp4mux");
  pipeline_elements.push_back(
      absl::StrFormat("filesink location=%s", options.file_path));
  return absl::StrJoin(pipeline_elements, " ! ");
}

}  // namespace

GstreamerVideoWriter::GstreamerVideoWriter(const Options& options)
    : options_(options) {}

absl::StatusOr<std::unique_ptr<GstreamerVideoWriter>>
GstreamerVideoWriter::Create(const Options& options) {
  VAI_RETURN_IF_ERROR(ValidateOptions(options));
  auto video_writer = std::make_unique<GstreamerVideoWriter>(options);
  VAI_RETURN_IF_ERROR(video_writer->Initialize());
  return std::move(video_writer);
}

absl::Status GstreamerVideoWriter::Initialize() {
  media_type_ = MediaTypeFromCaps(options_.caps_string);
  VAI_ASSIGN_OR_RETURN(frame_rate_,
                   GetFramerateFractionFromCaps(options_.caps_string));
  if (options_.h264_only && media_type_ != "video/x-h264") {
    return absl::FailedPreconditionError("Please provide h264 encoded input");
  }

  GstreamerRunner::Options gstreamer_runner_options;
  if (options_.h264_mux_only && media_type_ == "video/x-h264") {
    VAI_ASSIGN_OR_RETURN(pipeline_str_, AssembleH264MuxPipeline(options_));
    gstreamer_runner_options.appsrc_do_timestamps = false;
  } else {
    VAI_ASSIGN_OR_RETURN(pipeline_str_,
                     AssembleTranscodeMuxPipeline(options_, frame_rate_));
    gstreamer_runner_options.appsrc_do_timestamps = true;
  }
  gstreamer_runner_options.appsrc_caps_string = options_.caps_string;
  gstreamer_runner_options.processing_pipeline_string = pipeline_str_;
  LOG(INFO) << "Launching the gstreamer pipeline: "
            << gstreamer_runner_options.processing_pipeline_string;
  LOG(INFO) << "Accepting the caps string: "
            << gstreamer_runner_options.appsrc_caps_string;
  VAI_ASSIGN_OR_RETURN(gstreamer_runner_,
                   GstreamerRunner::Create(gstreamer_runner_options));
  return absl::OkStatus();
}

absl::Status GstreamerVideoWriter::Put(
    const GstreamerBuffer& gstreamer_buffer) {
  return gstreamer_runner_->Feed(gstreamer_buffer);
}

}  // namespace visionai
