/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_UTIL_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_UTIL_H_

#include <vector>

#include "absl/status/status.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/array/array3d.h"
#include "visionai/util/gtl/circularbuffer.h"

namespace visionai {
namespace motion_detection {

// Get the average of a vector.
float VectorAverage(const std::vector<float>& vec);

// Estimate the entropy of an array, given the mean and the standard deviation
// of this array.
float EstimateEntropy(const std::vector<float>& numbers, float mean, float std);

// Extract features from motion vectors for a video with regard to spatial
// grid
absl::Status ComputeMotionVectorFeaturesWithSpatialGrid(
    const MotionVectors& motion_vectors,
    gtl::CircularBuffer<Array3D<float>>& grids_features_buffer, int num_grid,
    int frame_height, int frame_width, std::vector<float>& mv_features);

// Extract feature from motion vectors
std::vector<float> ComputeMotionVectorFeatures(
    const MotionVectors& motion_vectors, int num_grid, int frame_height,
    int frame_width);

// Extract max magnitude entropy from motion vectors
absl::Status ComputeMaxMagnitudeEntropyWithSpatialGrid(
    const MotionVectors& motion_vectors,
    gtl::CircularBuffer<Array3D<float>>& grids_features_buffer, int num_grid,
    int frame_height, int frame_width, std::vector<float>& mv_features);
std::vector<float> ComputeMaxMagnitudeEntropyFeatures(
    const MotionVectors& motion_vectors, int num_grid, int frame_height,
    int frame_width);

}  // namespace motion_detection
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_UTIL_H_
