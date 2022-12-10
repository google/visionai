// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/captures/rtsp_capture.h"

#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/algorithms/media/util/util.h"

namespace visionai {

std::string RTSPCapture::GstPipelineStr() {
  std::vector<std::string> gst_pipeline;
  gst_pipeline.push_back(absl::StrFormat(
      "rtspsrc location=%s protocols=GST_RTSP_LOWER_TRANS_TCP", source_uri_));
  gst_pipeline.push_back("rtph264depay");
  gst_pipeline.push_back("h264parse");
  return absl::StrJoin(gst_pipeline, " ! ");
}

absl::Status RTSPCapture::Init(CaptureInitContext* ctx) {
  VAI_RETURN_IF_ERROR(GstInit());
  VAI_RETURN_IF_ERROR(ctx->GetInputUrl(&source_uri_));
  if (source_uri_.empty()) {
    return absl::InvalidArgumentError("Given an empty rtsp uri.");
  }
  VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("timeout", &timeout_seconds_));
  return absl::OkStatus();
}

// TODO(chenyangwei): Migrate the H264 gstreamer validator to the util.
absl::Status RTSPCapture::IsRTSPH264Input() {
  GstreamerRunner::Options pipeline_opts;
  std::string caps_string;
  pipeline_opts.processing_pipeline_string = absl::StrFormat(
      "rtspsrc location=%s protocols=GST_RTSP_LOWER_TRANS_TCP ! "
      "application/x-rtp,media=video",
      source_uri_);
  pipeline_opts.receiver_callback =
        [&](GstreamerBuffer buffer) -> absl::Status {
      caps_string = buffer.caps_string();
      // Once the pipeline received the first packet, pause/halt the pipeline.
      return absl::CancelledError();
    };

  VAI_RETURN_IF_ERROR(ExecuteH264ValidatorGstreamerRunner(pipeline_opts));

  VAI_RETURN_IF_ERROR(IsRTPCapsVideoH264(caps_string, source_uri_));

  return absl::OkStatus();
}

absl::Status RTSPCapture::ExecuteH264ValidatorGstreamerRunner(
    const GstreamerRunner::Options& options) {
  VAI_ASSIGN_OR_RETURN(auto pipeline, GstreamerRunner::Create(options));
  while (!is_cancelled_.HasBeenNotified() && !pipeline->IsCompleted()) {
  }
  return absl::OkStatus();
}

absl::Status RTSPCapture::Run(CaptureRunContext* ctx) {
  VAI_RETURN_IF_ERROR(IsRTSPH264Input());
  GstreamerRunner::Options pipeline_opts;
  // Only accepts the buffer after the first key frame has appeared.
  bool received_key_frame = false;
  pipeline_opts.processing_pipeline_string = GstPipelineStr();
  pipeline_opts.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    if (buffer.media_type() == "video/x-h264") {
      if (buffer.get_dts() == -1) {
        // Need to fill in the DTS value for the downstream muxers to perform
        // properly.
        if (last_frame_duration_ != -1) {
          // Prioritize to use the last frame's dts and duration to determine
          // the current frame's dts.
          buffer.set_dts(last_frame_dts_ + last_frame_duration_);
        } else {
          // The frame's duration could be unset in variable frame rate mode.
          // Then we set dts to be the same as pts.
          if (buffer.get_pts() <= last_frame_pts_) {
            LOG(WARNING)
                << "This is likely an input of variable frame rate with "
                   "B-frames. We will expand the support to this case soon.";
          }
          buffer.set_dts(buffer.get_pts());
        }
      }
      last_frame_pts_ = buffer.get_pts();
      last_frame_dts_ = buffer.get_dts();
      last_frame_duration_ = buffer.get_duration();
    }
    VLOG(4) << absl::StrFormat(
        "RTSPCapture received packet from RTSP: pts=%d, dts=%d, duration=%d",
        buffer.get_pts(), buffer.get_dts(), buffer.get_duration());

    received_key_frame = received_key_frame || buffer.is_key_frame();
    if (received_key_frame) {
      VAI_ASSIGN_OR_RETURN(auto p, MakePacket(std::move(buffer)));
      VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
    }
    return absl::OkStatus();
  };

  return ExecuteGstreamerRunner(pipeline_opts);
}

absl::Status RTSPCapture::ExecuteGstreamerRunner(
    const GstreamerRunner::Options& options) {
  VAI_ASSIGN_OR_RETURN(auto pipeline, GstreamerRunner::Create(options));
  while (!is_cancelled_.HasBeenNotified() && !pipeline->IsCompleted()) {
  }
  pipeline->SignalEOS();
  return absl::OkStatus();
}

// Arrange for the possibility for cancellation.
absl::Status RTSPCapture::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

REGISTER_CAPTURE_INTERFACE("RTSPCapture")
    .OutputPacketType("GstreamerBuffer")
    .Attr("timeout", "int")
    .Doc("RTSPCapture receives encoded frames from an RTSP uri source.");

REGISTER_CAPTURE_IMPLEMENTATION("RTSPCapture", visionai::RTSPCapture);

}  // namespace visionai
