// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/captures/file_source_capture.h"

#include <limits>

#include "google/protobuf/struct.pb.h"
#include "absl/status/status.h"
#include "absl/strings/match.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/codec_validator.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/streams/constants.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/gstreamer/pipeline_string.h"

namespace visionai {

absl::Status FileSourceCapture::Init(CaptureInitContext* ctx) {
  VAI_RETURN_IF_ERROR(ctx->GetInputUrl(&source_uri_));
  // TODO: Probably better to check if that file exists on the system.
  // This will take care of the empty string case too.
  if (source_uri_.empty()) {
    return absl::InvalidArgumentError("Given an empty filepath");
  } else if (!FileExists(source_uri_).ok()) {
    return absl::InvalidArgumentError(
        absl::StrFormat("No such file \"%s\"", source_uri_));
  }

  bool loop = false;
  VAI_RETURN_IF_ERROR(ctx->GetAttr<bool>("loop", &loop))
      << "while getting the \"loop\" attribute";
  if (loop) {
    VAI_RETURN_IF_ERROR(ctx->GetAttr<int64_t>("loop_count", &loop_count_))
        << "while getting the \"loop_count\" attribute";
    if (loop_count_ <= 0) {
      LOG(WARNING) << "Found non-positive loop_count: " << loop_count_;
    }
  } else {
    loop_count_ = 1;
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("timeout_sec", &timeout_sec_))
      << "while getting the \"timeout_sec\" attribute";
  if (timeout_sec_ < 0) {
    return absl::InvalidArgumentError(
        "Please specify a non-negative timeout value.");
  }

  return absl::OkStatus();
}

absl::Status FileSourceCapture::Run(CaptureRunContext* ctx) {
  absl::Duration total_duration_before_this_iteration;
  absl::Time last_updated_time = absl::Now();
  VAI_RETURN_IF_ERROR(IsSupportedMediaType(source_uri_));
  while (loop_count_-- > 0 && !is_cancelled_.HasBeenNotified()) {
    absl::Duration this_duration;
    GstreamerRunner::Options pipeline_opts;
    pipeline_opts.processing_pipeline_string =
        FileSrcGstPipelineStr(source_uri_);
    pipeline_opts.receiver_callback =
        [&](GstreamerBuffer buffer) -> absl::Status {
      // Accumulate the duration of the loop so the subsequent loops' timestmap
      // continues.
      this_duration += absl::Nanoseconds(buffer.get_duration());
      // Adjust the dts & pts with the accumulated duratons from previous
      // iterations.
      buffer.set_dts(
          buffer.get_dts() +
          absl::ToInt64Nanoseconds(total_duration_before_this_iteration));
      buffer.set_pts(
          buffer.get_pts() +
          absl::ToInt64Nanoseconds(total_duration_before_this_iteration));

      VAI_ASSIGN_OR_RETURN(auto p, MakePacket(std::move(buffer)));
      VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
      last_updated_time = absl::Now();
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
      if (absl::Now() - last_updated_time > absl::Seconds(timeout_sec_)) {
        pipeline->SignalEOS();
        return absl::DeadlineExceededError(absl::StrFormat(
            "Could not receive data for %ds; terminating.", timeout_sec_));
      }
    }
    pipeline->SignalEOS();
    total_duration_before_this_iteration += this_duration;
  }
  return absl::OkStatus();
}

// Arrange for the possibility for cancellation.
absl::Status FileSourceCapture::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

REGISTER_CAPTURE_INTERFACE("FileSourceCapture")
    .OutputPacketType("GstreamerBuffer")
    .Attr("loop", "bool")
    .Attr("loop_count", "int")
    .Attr("timeout_sec", "int")
    .Doc(R"doc(
FileSourceCapture reads from local video files and outputs encoded frames.

loop: When the video reaches the end, loop back to the beginning and play again.
)doc");

REGISTER_CAPTURE_IMPLEMENTATION("FileSourceCapture", FileSourceCapture);

}  // namespace visionai
