// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/detection/motion_detection/util.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/array/array3d.h"
#include "visionai/util/gtl/circularbuffer.h"

namespace visionai {
namespace motion_detection {
namespace {

TEST(MotionVectorUtilTest, CanProcessEmptyMotionVectors) {
  int frame_width = 1;
  int frame_height = 2;
  int num_grid = 1;
  int feature_number = 14;
  int temporal_buffer_size = 3;

  std::vector<float> features =
      ComputeMotionVectorFeatures({}, num_grid, frame_height, frame_width);
  EXPECT_EQ(features.size(), feature_number);

  gtl::CircularBuffer<Array3D<float>> grids_features_buffer(
      temporal_buffer_size);
  features.resize(num_grid * num_grid * feature_number * 2, 0.0);

  absl::Status status = ComputeMotionVectorFeaturesWithSpatialGrid(
      {}, grids_features_buffer, num_grid, frame_height, frame_width, features);

  EXPECT_TRUE(status.ok());
  EXPECT_EQ(features.size(), feature_number * 2);
}

TEST(MotionVectorUtilTest, CanComputeMotionVectorFeatures) {
  MotionVector mv1{/* .source = */ -1,
                   /* .w = */ 16,
                   /* .h = */ 16,
                   /* .src_x = */ 0,
                   /* .src_y = */ 0,
                   /* .dst_x = */ 0,
                   /* .dst_y = */ 0,
                   /* .motion_x = */ 0,
                   /* .motion_y = */ 1,
                   /* .motion_scale = */ 1};

  MotionVector mv2{/* .source = */ 1,
                   /* .w = */ 8,
                   /* .h = */ 8,
                   /* .src_x = */ 0,
                   /* .src_y = */ 0,
                   /* .dst_x = */ 0,
                   /* .dst_y = */ 0,
                   /* .motion_x = */ 2,
                   /* .motion_y = */ 0,
                   /* .motion_scale = */ 2};

  int frame_width = 1;
  int frame_height = 1;
  int num_grid = 1;
  int feature_number = 14;
  std::vector<float> features =
      ComputeMotionVectorFeatures({}, num_grid, frame_height, frame_width);
  EXPECT_EQ(features.size(), num_grid * num_grid * feature_number);

  features = ComputeMotionVectorFeatures({mv1, mv2}, num_grid, frame_height,
                                         frame_width);
  EXPECT_EQ(features.size(), feature_number);
  EXPECT_FLOAT_EQ(features[0], 1.0);
  EXPECT_FLOAT_EQ(features[1], 1.0);
  EXPECT_FLOAT_EQ(features[2], std::sqrt(2) / 2);
  EXPECT_FLOAT_EQ(features[3], std::sqrt(2) / 2);
  EXPECT_FLOAT_EQ(features[4], std::acos(std::sqrt(2) / 2));
  EXPECT_FLOAT_EQ(features[5], std::acos(std::sqrt(2) / 2));
  EXPECT_FLOAT_EQ(features[6], 0.75);
  EXPECT_FLOAT_EQ(features[7], 0.75);
  EXPECT_FLOAT_EQ(features[8], 0.0);
  EXPECT_FLOAT_EQ(features[9], 0.0);
  EXPECT_FLOAT_EQ(features[10], 0.0);
  EXPECT_FLOAT_EQ(features[11], 512.0);
  EXPECT_FLOAT_EQ(features[12], 768.0);
  EXPECT_FLOAT_EQ(features[13], 512.0);
}

TEST(MotionVectorUtilTest, CanComputeMaxMagnitudeEntropywithSpatialGrid) {
  int frame_width = 10;
  int frame_height = 10;
  int num_grid = 1;
  int feature_number = 1;
  int temporal_buffer_size = 3;
  gtl::CircularBuffer<Array3D<float>> grids_features_buffer(
      temporal_buffer_size);

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
                    /* .dst_x = */ 6,
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

  // The first frame
  std::vector<float> features_with_grids1(num_grid * num_grid * feature_number,
                                          0.0);
  absl::Status status = ComputeMaxMagnitudeEntropyWithSpatialGrid(
      {mv00, mv01, mv10, mv11}, grids_features_buffer, num_grid, frame_height,
      frame_width, features_with_grids1);
  EXPECT_TRUE(status.ok());
  EXPECT_FLOAT_EQ(features_with_grids1.at(0), 1.3862944);

  // Next frame
  mv00 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 1,
                      /* .dst_y = */ 1,
                      /* .motion_x = */ 20,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  mv01 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 6,
                      /* .dst_y = */ 1,
                      /* .motion_x = */ 30,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  mv10 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 1,
                      /* .dst_y = */ 6,
                      /* .motion_x = */ 40,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  mv11 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 6,
                      /* .dst_y = */ 6,
                      /* .motion_x = */ 50,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  std::vector<float> features_with_grids2(num_grid * num_grid * feature_number,
                                          0.0);
  status = ComputeMaxMagnitudeEntropyWithSpatialGrid(
      {mv00, mv01, mv10, mv11}, grids_features_buffer, num_grid, frame_height,
      frame_width, features_with_grids2);
  EXPECT_TRUE(status.ok());
  EXPECT_FLOAT_EQ(features_with_grids1.at(0), 1.3862944);

  // Next frame is an I frame with no motion vectors
  std::vector<float> features_with_grids3(num_grid * feature_number, 0.0);
  status = ComputeMaxMagnitudeEntropyWithSpatialGrid(
      {}, grids_features_buffer, num_grid, frame_height, frame_width,
      features_with_grids3);
  EXPECT_TRUE(status.ok());
  EXPECT_FLOAT_EQ(features_with_grids1.at(0), 1.3862944);
}

TEST(MotionVectorUtilTest, CanComputeMotionVectorFeaturesWithSpatialGrid) {
  int frame_width = 10;
  int frame_height = 10;
  int num_grid = 2;
  int feature_number = 14;
  int temporal_buffer_size = 3;
  gtl::CircularBuffer<Array3D<float>> grids_features_buffer(
      temporal_buffer_size);

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
                    /* .dst_x = */ 6,
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

  // The first frame
  std::vector<float> features_with_grids1(
      feature_number * num_grid * num_grid * 2, 0.0);
  absl::Status status = ComputeMotionVectorFeaturesWithSpatialGrid(
      {mv00, mv01, mv10, mv11}, grids_features_buffer, num_grid, frame_height,
      frame_width, features_with_grids1);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(grids_features_buffer.size(), 1);
  EXPECT_FLOAT_EQ(features_with_grids1[0], 1.0);
  EXPECT_FLOAT_EQ(features_with_grids1[feature_number], 2.0);
  EXPECT_FLOAT_EQ(features_with_grids1[feature_number * 2], 3.0);
  EXPECT_FLOAT_EQ(features_with_grids1[feature_number * 3], 4.0);

  // Next frame
  mv00 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 1,
                      /* .dst_y = */ 1,
                      /* .motion_x = */ 20,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  mv01 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 6,
                      /* .dst_y = */ 1,
                      /* .motion_x = */ 30,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  mv10 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 1,
                      /* .dst_y = */ 6,
                      /* .motion_x = */ 40,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  mv11 = MotionVector{/* .source = */ 1,
                      /* .w = */ 16,
                      /* .h = */ 16,
                      /* .src_x = */ 0,
                      /* .src_y = */ 0,
                      /* .dst_x = */ 6,
                      /* .dst_y = */ 6,
                      /* .motion_x = */ 50,
                      /* .motion_y = */ 0,
                      /* .motion_scale = */ 1};

  std::vector<float> features_with_grids2(
      num_grid * num_grid * feature_number * 2, 0.0);
  status = ComputeMotionVectorFeaturesWithSpatialGrid(
      {mv00, mv01, mv10, mv11}, grids_features_buffer, num_grid, frame_height,
      frame_width, features_with_grids2);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(grids_features_buffer.size(), 2);
  // Running average of features
  EXPECT_FLOAT_EQ(features_with_grids2[0], 1.5);
  EXPECT_FLOAT_EQ(features_with_grids2[feature_number], 2.5);
  EXPECT_FLOAT_EQ(features_with_grids2[feature_number * 2], 3.5);
  EXPECT_FLOAT_EQ(features_with_grids2[feature_number * 3], 4.5);
  // Running std of features
  EXPECT_FLOAT_EQ(features_with_grids2[feature_number * 4], 0.5);
  EXPECT_FLOAT_EQ(features_with_grids2[feature_number * 5], 0.5);
  EXPECT_FLOAT_EQ(features_with_grids2[feature_number * 6], 0.5);
  EXPECT_FLOAT_EQ(features_with_grids2[feature_number * 7], 0.5);

  // Next frame is an I frame with no motion vectors
  std::vector<float> features_with_grids3(
      num_grid * num_grid * feature_number * 2, 0.0);
  status = ComputeMotionVectorFeaturesWithSpatialGrid(
      {}, grids_features_buffer, num_grid, frame_height, frame_width,
      features_with_grids3);
  EXPECT_TRUE(status.ok());
  EXPECT_EQ(grids_features_buffer.size(),
            2);  // buffer and running mean is unchanged.
  // Running average of features
  EXPECT_FLOAT_EQ(features_with_grids3[0], 1.5);
  EXPECT_FLOAT_EQ(features_with_grids3[feature_number], 2.5);
  EXPECT_FLOAT_EQ(features_with_grids3[feature_number * 2], 3.5);
  EXPECT_FLOAT_EQ(features_with_grids3[feature_number * 3], 4.5);
}

}  // namespace
}  // namespace motion_detection
}  // namespace visionai
