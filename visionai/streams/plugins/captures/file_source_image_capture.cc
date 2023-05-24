// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/captures/file_source_image_capture.h"

#include "google/protobuf/struct.pb.h"
#include "absl/status/status.h"
#include "absl/strings/str_join.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/codec_validator.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/streams/constants.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/producer_consumer_queue.h"

namespace visionai {

std::string FileSourceImageCapture::GstPipelineStr() {
  // The GStreamer pipeline receives media data from a local file and pushes the
  // raw image packets to downstream.
  std::vector<std::string> gst_pipeline;
  gst_pipeline.push_back(absl::StrFormat("filesrc location=%s", source_uri_));
  gst_pipeline.push_back("decodebin");
  gst_pipeline.push_back("videoconvert");
  gst_pipeline.push_back("video/x-raw,format=RGB");

  if (!frame_rate_.empty()) {
    gst_pipeline.push_back("videorate");
    gst_pipeline.push_back(
        absl::StrFormat("video/x-raw,framerate=%s", frame_rate_));
  }
  return absl::StrJoin(gst_pipeline, " ! ");
}

absl::Status FileSourceImageCapture::Init(CaptureInitContext* ctx) {
  VAI_RETURN_IF_ERROR(ctx->GetInputUrl(&source_uri_));
  // TODO: Probably better to check if that file exists on the system.
  // This will take care of the empty string case too.
  if (source_uri_.empty()) {
    return absl::InvalidArgumentError("Given an empty filepath");
  } else if (!FileExists(source_uri_).ok()) {
    return absl::InvalidArgumentError(
      absl::StrFormat("No such file \"%s\"", source_uri_));
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("frame_rate", &frame_rate_))
      << "while getting the \"frame_rate\" attribute";
  VAI_RETURN_IF_ERROR(ctx->GetAttr<bool>("loop", &loop_))
      << "while getting the \"loop\" attribute";
  return absl::OkStatus();
}

absl::Status FileSourceImageCapture::Run(CaptureRunContext* ctx) {
  VAI_RETURN_IF_ERROR(IsVideoH264Input(source_uri_));
  do {
    GstreamerRunner::Options pipeline_opts;
    pipeline_opts.processing_pipeline_string = GstPipelineStr();
    pipeline_opts.receiver_callback =
        [&](GstreamerBuffer buffer) -> absl::Status {
      VAI_ASSIGN_OR_RETURN(auto image, ToRawImage(std::move(buffer)));
      VAI_ASSIGN_OR_RETURN(auto p, MakePacket(std::move(image)));
      VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
      VLOG(2) << "FileSourceImageCapture decoded one image.";
      return absl::OkStatus();
    };

    // Make video file play at playback rate.
    //
    // TODO(b/234659862): setting this to false allows immediate reads.
    // Try to see if we can remove this for GA.
    pipeline_opts.appsink_sync = true;

    // Once created, the GStreamer pipeline will run continuously in the
    // background.
    VAI_ASSIGN_OR_RETURN(auto pipeline, GstreamerRunner::Create(pipeline_opts));
    while (!is_cancelled_.HasBeenNotified() &&
           !pipeline->WaitUntilCompleted(
               absl::Milliseconds(kDefaultCapturePollCompletionIntervalMs))) {
    }
    pipeline->SignalEOS();
  } while (loop_ && !is_cancelled_.HasBeenNotified());
  return absl::OkStatus();
}

// Arrange for the possibility for cancellation.
absl::Status FileSourceImageCapture::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

REGISTER_CAPTURE_INTERFACE("FileSourceImageCapture")
    .OutputPacketType("GstreamerBuffer")
    .Attr("loop", "bool")
    .Attr("frame_rate", "string")
    .Doc(R"doc(
FileSourceImageCapture reads from local video files and outputs decoded frames.

loop: When the video reaches the end, loop back to the beginning and play again.
frame_rate: Optional output frame rate, with the format ("%d/%d", number_of_frames, number_of_seconds)
)doc");

REGISTER_CAPTURE_IMPLEMENTATION("FileSourceImageCapture",
                                visionai::FileSourceImageCapture);

}  // namespace visionai
