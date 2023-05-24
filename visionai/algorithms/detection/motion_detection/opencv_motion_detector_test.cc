// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

// File ported from
// google3/research/soapbox/image_process/opencv_motion_detector_test.cc
// PORT_NOTE: General port notes follow:
//  - Used OpenCV Mat instead of WImage.
//  - Changed top-level namespaces.
#include "visionai/algorithms/detection/motion_detection/opencv_motion_detector.h"

#include <cstdint>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/imgcodecs.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "visionai/util/file_path.h"

namespace visionai {
namespace motion_detection {
namespace {

const char kTestFolder[] = "visionai/testing/testdata/media/motion";
constexpr int kNumTestImages = 20;

// Tests that the motion detection interface works, by loading some image frames
// and verifying the foreground masks against expected results.
TEST(OpenCVMotionDetectorTest, TestComputeForeground) {
  OpenCVMotionDetectorConfig config;
  OpenCVMotionDetector detector(config);

  for (int i = 0; i < kNumTestImages; ++i) {
    std::string filename =
        absl::StrCat("frame01", absl::StrFormat("%02d", i), ".jpg");
    cv::Mat image_frame = cv::imread(file::JoinPath(kTestFolder, filename));
    ASSERT_FALSE(image_frame.empty());

    cv::Mat foreground;
    ASSERT_TRUE(detector.ComputeForeground(image_frame, &foreground).ok());

    filename = absl::StrCat("foreground01", absl::StrFormat("%02d", i), ".png");
    cv::Mat expected_foreground =
        cv::imread(file::JoinPath(kTestFolder, filename), cv::IMREAD_GRAYSCALE);
    EXPECT_LE(cv::countNonZero(foreground != expected_foreground), 2);
  }
}

// Tests that motion detection from foreground masks work.
TEST(OpenCVMotionDetectorTest, TestMotionDetectionFromForegroundMask) {
  OpenCVMotionDetectorConfig config;
  OpenCVMotionDetector detector(config);

  const int width = 8;
  const int height = 5;
  const int max_intensity = 255;
  const int min_intensity = 0;

  cv::Mat image_frame1(height, width, CV_8UC1, cv::Scalar(max_intensity));
  cv::Mat image_frame2(height, width, CV_8UC1, cv::Scalar(min_intensity));

  EXPECT_TRUE(detector.MotionDetectionFromForegroundMask(image_frame1));
  EXPECT_FALSE(detector.MotionDetectionFromForegroundMask(image_frame2));
}

}  // namespace
}  // namespace motion_detection
}  // namespace visionai
