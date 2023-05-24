// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// File ported from
// google3/research/soapbox/image_process/opencv_motion_detector.cc
// PORT_NOTE:
// General port notes follow:
//  - Changed top-level namespaces.

#include "visionai/algorithms/detection/motion_detection/opencv_motion_detector.h"

#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "opencv4/opencv2/video.hpp"
#include "absl/status/status.h"
#include "visionai/algorithms/detection/motion_detection/opencv_motion_detector_config.pb.h"
#include "visionai/algorithms/detection/motion_detection/util.h"
#include "visionai/types/raw_image.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace motion_detection {

OpenCVMotionDetector::OpenCVMotionDetector(
    const OpenCVMotionDetectorConfig &config)
    : config_(config) {
  background_subtractor_ = cv::createBackgroundSubtractorMOG2(
      config_.background_history_frame_length(),
      config_.variance_threshold_num_pix(), config_.shadow_detection());
}

OpenCVMotionDetector::~OpenCVMotionDetector() = default;

absl::Status OpenCVMotionDetector::ComputeForeground(const cv::Mat &image_frame,
                                                     cv::Mat *foreground) {
  CHECK(foreground != nullptr);

  // Compute the new size for the frame.
  int new_width = config_.scale() * image_frame.cols;
  int new_height = config_.scale() * image_frame.rows;
  if (new_width % 2 == 1) new_width += 1;
  if (new_height % 2 == 1) new_height += 1;

  if (new_width == 0 || new_height == 0) {
    return absl::InvalidArgumentError("resized image size = 0");
  }

  // Resize and convert the image to grayscale.
  cv::Mat input_image_scaled;
  cv::resize(image_frame, input_image_scaled, cv::Size(new_width, new_height),
             0, 0, cv::INTER_LINEAR);
  cv::Mat input_image_gray;
  if (image_frame.channels() > 1) {
    cv::cvtColor(input_image_scaled, input_image_gray, cv::COLOR_RGB2GRAY);
  } else {
    input_image_gray = input_image_scaled;
  }

  background_subtractor_->apply(input_image_gray, *foreground);
  return absl::OkStatus();
}

bool OpenCVMotionDetector::MotionDetectionFromForegroundMask(
    const cv::Mat &foreground_mask) {
  const float motion_area_ratio = ::visionai::motion_detection::MotionAreaRatio(
      foreground_mask, config_.motion_foreground_pixel_threshold());
  return static_cast<int>(motion_area_ratio > config_.motion_area_threshold());
}

absl::StatusOr<bool> OpenCVMotionDetector::DetectMotion(
    const RawImage &raw_image) {
  cv::Mat image_frame(raw_image.height(), raw_image.width(), CV_8UC3);
  std::memcpy(image_frame.data, raw_image.data(),
              image_frame.step[0] * image_frame.rows);
  cv::Mat foreground_mask;
  VAI_RETURN_IF_ERROR(ComputeForeground(image_frame, &foreground_mask));
  return MotionDetectionFromForegroundMask(foreground_mask);
}

}  // namespace motion_detection
}  // namespace visionai
