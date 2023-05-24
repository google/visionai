/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

// File ported from
// google3/research/soapbox/image_process/opencv_motion_detector.h PORT_NOTE:
// General port notes follow:
//  - Changed top-level namespaces.

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_OPENCV_MOTION_DETECTOR_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_OPENCV_MOTION_DETECTOR_H_

// A thin wrapper around OpenCV's motion detector to perform background
// subtraction on consecutive video frames and return a foreground mask.
// Note: These modules are not thread-safe.

#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/video.hpp"
#include "absl/status/status.h"
#include "visionai/algorithms/detection/motion_detection/opencv_motion_detector_config.pb.h"
#include "visionai/types/raw_image.h"

namespace visionai {
namespace motion_detection {

class OpenCVMotionDetector {
 public:
  explicit OpenCVMotionDetector(const OpenCVMotionDetectorConfig &config);
  virtual ~OpenCVMotionDetector();

  // Performs motion detection and determines the foreground region in the
  // specified image_frame. Assumes that this method is called for subsequent
  // frames in a video stream, and returns a single channel 8bit binary
  // foreground (0 - background, 0xFF - foreground).
  absl::Status ComputeForeground(const cv::Mat &image_frame,
                                 cv::Mat *foreground);

  bool MotionDetectionFromForegroundMask(const cv::Mat &foreground_mask);

  // Main function to call to detect motion.
  absl::StatusOr<bool> DetectMotion(const RawImage &raw_image);

 private:
  OpenCVMotionDetectorConfig config_;

  // Background subtraction module.
  cv::Ptr<cv::BackgroundSubtractorMOG2> background_subtractor_;
};

}  // namespace motion_detection
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_OPENCV_MOTION_DETECTOR_H_
