// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/captures/rtsp_image_capture.h"

#include "google/protobuf/struct.pb.h"
#include "absl/strings/str_join.h"
#include "absl/synchronization/notification.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "visionai/algorithms/media/util/gstreamer_runner.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/util/producer_consumer_queue.h"

namespace visionai {

std::string RTSPImageCapture::InputGstPipelineStr() {
  std::vector<std::string> gst_pipeline;
  gst_pipeline.push_back(absl::StrFormat(
      "rtspsrc location=%s protocols=GST_RTSP_LOWER_TRANS_TCP", source_uri_));
  gst_pipeline.push_back("rtph264depay");
  return absl::StrJoin(gst_pipeline, " ! ");
}

std::string RTSPImageCapture::OutputGstPipelineStr() {
  std::vector<std::string> gst_pipeline;
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

absl::Status RTSPImageCapture::Init(CaptureInitContext* ctx) {
  VAI_RETURN_IF_ERROR(ctx->GetInputUrl(&source_uri_));
  if (source_uri_.empty()) {
    return absl::InvalidArgumentError("Given an empty rtsp uri.");
  }
  VAI_RETURN_IF_ERROR(
      ctx->GetAttr<int>("max_captured_images", &max_captured_images_));
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("camera_id", &camera_id_));
  VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("buffer_size", &buffer_size_));
  VAI_RETURN_IF_ERROR(ctx->GetAttr<std::string>("frame_rate", &frame_rate_));
  VAI_RETURN_IF_ERROR(ctx->GetAttr<int>("timeout", &timeout_seconds_));
  return absl::OkStatus();
}

// TODO(chenyangwei): Migrate the H264 gstreamer validator to the util.
absl::Status RTSPImageCapture::IsRTSPH264Input() {
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

  VAI_ASSIGN_OR_RETURN(auto pipeline, GstreamerRunner::Create(pipeline_opts));
  while (!is_cancelled_.HasBeenNotified() && !pipeline->IsCompleted()) {
  }

  VAI_RETURN_IF_ERROR(IsRTPCapsVideoH264(caps_string, source_uri_));

  return absl::OkStatus();
}

absl::Status RTSPImageCapture::Run(CaptureRunContext* ctx) {
  VAI_RETURN_IF_ERROR(IsRTSPH264Input());

  ProducerConsumerQueue<GstreamerBuffer> queue(buffer_size_);

  GstreamerRunner::Options input_pipeline_opts, output_pipeline_opts;
  // The input pipeline receives media data from RTSP server and pushes the
  // buffer into a queue.
  // It only accepts the buffer after the first key frame has appeared.
  bool received_key_frame;
  input_pipeline_opts.processing_pipeline_string = InputGstPipelineStr();
  input_pipeline_opts.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    received_key_frame = received_key_frame || buffer.is_key_frame();
    if (received_key_frame) {
      queue.TryEmplace(std::move(buffer));
    }
    return absl::OkStatus();
  };
  // The output pipeline decodes the media data and pushes the raw image
  // packets to downstream.
  // It stops pushing after the max captured image is reached.
  int captured_images = 0;
  output_pipeline_opts.processing_pipeline_string = OutputGstPipelineStr();
  output_pipeline_opts.receiver_callback =
      [&](GstreamerBuffer buffer) -> absl::Status {
    if (captured_images >= max_captured_images_) {
      return absl::OkStatus();
    }
    VAI_ASSIGN_OR_RETURN(auto image, ToRawImage(std::move(buffer)));
    VAI_ASSIGN_OR_RETURN(auto p, MakePacket(std::move(image)));
    google::protobuf::Value value;
    value.set_string_value(camera_id_);
    VAI_RETURN_IF_ERROR(SetMetadataField("camera_id", value, &p));
    VAI_RETURN_IF_ERROR(ctx->Push(std::move(p)));
    captured_images++;
    return absl::OkStatus();
  };

  // Configure the output pipeline's appsrc cap string by getting the
  // first buffer message from the input pipeline.
  VAI_ASSIGN_OR_RETURN(auto input_pipeline,
                   GstreamerRunner::Create(input_pipeline_opts));
  GstreamerBuffer buffer;
  if (!queue.TryPop(buffer, absl::Seconds(timeout_seconds_))) {
    return absl::UnknownError(
        "Could not receive data from RTSP input pipeline.");
  }
  output_pipeline_opts.appsrc_caps_string = buffer.caps_string();
  VAI_ASSIGN_OR_RETURN(auto output_pipeline,
                   GstreamerRunner::Create(output_pipeline_opts));

  // Feed the buffers to the output pipeline.
  while (!is_cancelled_.HasBeenNotified()) {
    VAI_RETURN_IF_ERROR(output_pipeline->Feed(buffer));
    if (!queue.TryPop(buffer, absl::Seconds(timeout_seconds_))) {
      break;
    }
    if (captured_images >= max_captured_images_) {
      break;
    }
  }
  return absl::OkStatus();
}

// Arrange for the possibility for cancellation.
absl::Status RTSPImageCapture::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

REGISTER_CAPTURE_INTERFACE("RTSPImageCapture")
    .OutputPacketType("RawImage")
    // The maximum number of images to be captured.
    .Attr("max_captured_images", "int")
    // The camera id to be set in the packet metadata.
    .Attr("camera_id", "string")
    // Optional: the internal buffer size of the capture. Default to 100.
    .Attr("buffer_size", "int")
    // Optional: the output frame rate, with the format ("%d/%d",
    // number_of_frames, number_of_seconds).
    .Attr("frame_rate", "string")
    // Optional: the timeout (in seconds) to receive contents from the RTSP
    // source. Default to 10s.
    .Attr("timeout", "int")
    .Doc(
        "RTSPImageCapture receives media data from an RTSP uri source. It "
        "drops the initial several non-key frame packets and converts the "
        "media data to the raw image format. The capture will finish its work "
        "after the max_captured_images is reached");

REGISTER_CAPTURE_IMPLEMENTATION("RTSPImageCapture", visionai::RTSPImageCapture);

}  // namespace visionai
