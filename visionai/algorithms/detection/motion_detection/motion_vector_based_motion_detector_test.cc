// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector.h"

#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector_config.pb.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/array/array3d.h"
#include "visionai/util/gtl/circularbuffer.h"

namespace visionai {
namespace motion_detection {
namespace {

constexpr int kGridNumber = 3;
constexpr int kFeatureNumber = 1;
constexpr float kMotionSensitivity = 0.3;

// Test that motion detection interface works by sending empty mv
TEST(MotionVectorBasedMotionDetectorTest, CanProcessEmptyMotionVectors) {
  MotionVectorBasedMotionDetectorConfig config;
  MotionVectorBasedMotionDetector detector(config, 1, 2);

  bool hasMotion = detector.DetectMotion({});
  EXPECT_FALSE(hasMotion);
}

// Test to see if motion detector can detect motion
// There is motion in input data
TEST(MotionVectorBasedMotionDetectorTest, CanDetectMotion) {
  MotionVector mv00{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 1,
                    /* .dst_y = */ 1,
                    /* .motion_x = */ 10,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVector mv01{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 2,
                    /* .dst_y = */ 1,
                    /* .motion_x = */ 20,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVector mv10{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 1,
                    /* .dst_y = */ 6,
                    /* .motion_x = */ 30,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVector mv11{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 6,
                    /* .dst_y = */ 6,
                    /* .motion_x = */ 40,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVectorBasedMotionDetectorConfig config;
  config.set_spatial_grid_number(kGridNumber);
  config.set_motion_sensitivity(kMotionSensitivity);
  MotionVectorBasedMotionDetector detector(config, 10, 10);

  EXPECT_TRUE(detector.DetectMotion({mv00, mv01, mv10, mv11}));
}

// Test to see if motion detector can return motion vector feature
// There is motion in input data
TEST(MotionVectorBasedMotionDetectorTest, CanReturnMotionVectorFeatures) {
  MotionVector mv00{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 1,
                    /* .dst_y = */ 1,
                    /* .motion_x = */ 10,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVector mv01{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 2,
                    /* .dst_y = */ 1,
                    /* .motion_x = */ 20,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVector mv10{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 1,
                    /* .dst_y = */ 6,
                    /* .motion_x = */ 30,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVector mv11{/* .source = */ 1,
                    /* .w = */ 16,
                    /* .h = */ 16,
                    /* .src_x = */ 0,
                    /* .src_y = */ 0,
                    /* .dst_x = */ 6,
                    /* .dst_y = */ 6,
                    /* .motion_x = */ 40,
                    /* .motion_y = */ 0,
                    /* .motion_scale = */ 1};

  MotionVectorBasedMotionDetectorConfig config;
  config.set_spatial_grid_number(kGridNumber);
  config.set_motion_sensitivity(kMotionSensitivity);
  MotionVectorBasedMotionDetector detector(config, 10, 10);

  std::vector<float> mv_features = detector.GetMotionVectorFeatures();
  EXPECT_THAT(mv_features.size(), 0);
  std::vector<float> empty_features;
  EXPECT_TRUE(mv_features.empty());

  // run one set of detect motion to get mv feature populated
  EXPECT_TRUE(detector.DetectMotion({mv00, mv01, mv10, mv11}));

  mv_features = detector.GetMotionVectorFeatures();
  EXPECT_THAT(mv_features.size(), kGridNumber * kGridNumber * kFeatureNumber);
}

}  // namespace
}  // namespace motion_detection
}  // namespace visionai
