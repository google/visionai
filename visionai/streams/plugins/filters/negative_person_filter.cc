// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/plugins/filters/negative_person_filter.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "glog/logging.h"
#include "opencv2/core/core.hpp"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "absl/synchronization/notification.h"
#include "absl/time/time.h"
#include "visionai/algorithms/detection/object_detection/object_detector.h"
#include "visionai/algorithms/media/util/type_util.h"
#include "visionai/streams/framework/filter.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/file_helpers.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {

namespace {
using ::visionai::object_detection::Detection;
using ::visionai::object_detection::ObjectDetector;
constexpr char kDefaultGraphPath[] =
    "visionai/testing/testdata/models/person/"
    "frozen_inference_graph_rcnn_inception_resnet.pb";
constexpr char kDefaultLabelMapPath[] =
    "visionai/testing/testdata/models/person/"
    "person_only_label_map_rcnn_inception_resnet.pbtxt";
constexpr char kDefaultInputLayerName[] = "image_tensor";
constexpr char kDefaultOutputLayerNames[] =
    "detection_boxes,detection_scores,detection_classes,num_detections";
constexpr int kDefaultMaxDetectionsToOutput = 500;
constexpr float kDefaultMinScoreThreshToOutput = 0.1;
constexpr float kDefaultTimeOutMs = 10000.0;  // 10 seconds
}  // namespace

absl::Status NegativePersonFilter::Init(FilterInitContext* ctx) {
  // Set default configurations for the person filter.
  std::string graph_path = kDefaultGraphPath;
  std::string label_map_path = kDefaultLabelMapPath;
  std::string input_layer_name = kDefaultInputLayerName;
  std::string raw_output_layer_names = kDefaultOutputLayerNames;
  std::vector<std::string> output_layer_names;
  int max_detections_to_output = kDefaultMaxDetectionsToOutput;
  float min_score_thresh_to_output = kDefaultMinScoreThreshToOutput;
  float time_out_ms = kDefaultTimeOutMs;

  // Get configurations from `ctx` and then validate the parameters, reset to
  // default if necessary.
  VAI_RETURN_IF_ERROR(ctx->GetAttr("graph_path", &graph_path));
  if (!FileExists(graph_path).ok() || !absl::EndsWith(graph_path, ".pb")) {
    LOG(WARNING) << absl::StrFormat(
        "The `graph_path` parameter must be directed to a valid binary "
        "protobuf file to initialize the TensorFlow model. Got %s instead. "
        "Reset to default: %s.",
        graph_path, kDefaultGraphPath);
    graph_path = kDefaultGraphPath;
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("label_map_path", &label_map_path));
  if (!FileExists(label_map_path).ok() ||
      !absl::EndsWith(label_map_path, ".pbtxt")) {
    LOG(WARNING) << absl::StrFormat(
        "The `label_map_path` parameter must be directed to a valid text "
        "protobuf file to specify map the labels. Got %s instead. Reset to "
        "default: %s.",
        label_map_path, kDefaultLabelMapPath);
    label_map_path = kDefaultLabelMapPath;
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("input_layer_name", &input_layer_name));
  if (input_layer_name.empty()) {
    LOG(WARNING) << absl::StrFormat(
        "The `input_layer_name` parameter cannot be empty and should be the "
        "input layer name for the TensorFlow model. Got %s instead. Reset to "
        "default: %s.",
        input_layer_name, kDefaultInputLayerName);
    input_layer_name = kDefaultInputLayerName;
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("output_layer_names", &raw_output_layer_names));
  if (raw_output_layer_names.empty() ||
      std::count(raw_output_layer_names.begin(), raw_output_layer_names.end(),
                 ',') < 4) {
    LOG(WARNING) << absl::StrFormat(
        "The `output_layer_names` parameter cannot be empty and should be the "
        "the output layer names for **4** fields (detection boxes, detection "
        "scores, detection classes, num of detections), seperated by commas. "
        "Got %s instead. Reset to default: %s.",
        raw_output_layer_names, kDefaultOutputLayerNames);
    raw_output_layer_names = kDefaultOutputLayerNames;
  }

  VAI_RETURN_IF_ERROR(
      ctx->GetAttr("max_detections_to_output", &max_detections_to_output));
  if (max_detections_to_output < 0) {
    LOG(WARNING) << absl::StrFormat(
        "The `max_detections_to_output` parameter cannot be smaller than 0. "
        "Got %d instead. Reset to default: %d.",
        max_detections_to_output, kDefaultMaxDetectionsToOutput);
    max_detections_to_output = kDefaultMaxDetectionsToOutput;
  }

  VAI_RETURN_IF_ERROR(
      ctx->GetAttr("min_score_thresh_to_output", &min_score_thresh_to_output));
  if (min_score_thresh_to_output < 0 || min_score_thresh_to_output > 1) {
    LOG(WARNING) << absl::StrFormat(
        "The `min_score_thresh_to_output` parameter cannot be in [0, 1]. "
        "Got %f instead. Reset to default: %f.",
        min_score_thresh_to_output, kDefaultMinScoreThreshToOutput);
    min_score_thresh_to_output = kDefaultMinScoreThreshToOutput;
  }

  VAI_RETURN_IF_ERROR(ctx->GetAttr("time_out_ms", &time_out_ms));
  if (time_out_ms <= 0) {
    LOG(WARNING) << absl::StrFormat(
        "The `time_out_ms` parameter cannot be smaller than 0. Got %f instead. "
        "Reset to default: %f.",
        time_out_ms, time_out_ms);
    time_out_ms = kDefaultTimeOutMs;
  }

  // Postprocess some parameters.
  output_layer_names = absl::StrSplit(raw_output_layer_names, ',');
  poll_time_out_ = absl::Milliseconds(time_out_ms);

  // Set up person_detector with the parameters provided.
  person_detector_ = std::make_unique<ObjectDetector>(
      graph_path, label_map_path, input_layer_name, output_layer_names,
      max_detections_to_output, min_score_thresh_to_output);

  // Generate random input image to warm up the detector.
  const int kImageWidth = 64;
  const int kImageHeight = 64;
  cv::Mat warm_up_input(kImageWidth, kImageHeight, CVX_8UC3);
  cv::randu(warm_up_input, cv::Scalar::all(0), cv::Scalar::all(255));
  std::vector<Detection> detections;
  VAI_RETURN_IF_ERROR(person_detector_->DetectObjects(warm_up_input, &detections));

  return absl::OkStatus();
}

absl::Status NegativePersonFilter::Run(FilterRunContext* ctx) {
  VAI_ASSIGN_OR_RETURN(auto event_id, ctx->StartEvent());

  // Currently, only can handle the happy path scenario,
  // processing all the captured images even if the capture stops working.
  // For the unhappy path, if there is an error thrown by the
  // capture or event_writer, the filter wouldn't stop immediately.
  // It will keep processing the remaining captured images in the buffer
  // until until they have been processed. For the better error handling,
  // the filter needs to be terminated immediately when the error happens.
  //
  // TODO(chenyangwei): Add the immediate termination while
  // filter or event_writer throwing error.
  while (true) {
    Packet p;
    absl::Status status = ctx->Poll(&p, poll_time_out_);
    if (!status.ok()) {
      if (is_cancelled_.HasBeenNotified()) {
        return absl::OkStatus();
      } else {
        return status;
      }
    }
    PacketAs<RawImage> p_as_img(p);
    if (!p_as_img.status().ok()) {
      return p_as_img.status();
    }
    RawImage& raw_image = *p_as_img;

    std::vector<Detection> detections;
    VAI_RETURN_IF_ERROR(person_detector_->DetectObjects(raw_image, &detections));

    if (!detections.empty()) {
      LOG(WARNING) << "Person detected! Dropping the frame.";
    } else {
      VAI_ASSIGN_OR_RETURN(auto raw_image_gstreamer_buffer,
                       ToGstreamerBuffer(std::move(raw_image)));
      VAI_RETURN_IF_ERROR(Pack(std::move(raw_image_gstreamer_buffer), &p));
      VAI_RETURN_IF_ERROR(ctx->Push(event_id, std::move(p)));
    }
  }

  VAI_RETURN_IF_ERROR(ctx->EndEvent(event_id));
  return absl::OkStatus();
}

absl::Status NegativePersonFilter::Cancel() {
  is_cancelled_.Notify();
  return absl::OkStatus();
}

REGISTER_FILTER_INTERFACE("NegativePersonFilter")
    .InputPacketType("RawImage")
    .OutputPacketType("GStreamerBuffer")
    .Attr("graph_path", "string")
    .Attr("label_map_path", "string")
    .Attr("input_layer_name", "string")
    .Attr("output_layer_names", "string")
    .Attr("max_detections_to_output", "int")
    .Attr("min_score_thresh_to_output", "float")
    .Attr("time_out_ms", "float")
    .Doc(
        "NegativePersonFilter is to filter out frames that have people "
        "detected and only pass through the frames without people.");
REGISTER_FILTER_IMPLEMENTATION("NegativePersonFilter", NegativePersonFilter);

}  // namespace visionai
