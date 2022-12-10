// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector.h"

#include <vector>

#include "absl/status/status.h"
#include "visionai/algorithms/detection/motion_detection/util.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/array/array3d.h"
#include "visionai/util/gtl/circularbuffer.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace motion_detection {

MotionVectorBasedMotionDetector::MotionVectorBasedMotionDetector(
    const MotionVectorBasedMotionDetectorConfig& config, int width, int height)
    : config_(config),
      frame_width_(width),
      frame_height_(height),
      grids_features_buffer_(config.temporal_buffer_frames()) {}

MotionVectorBasedMotionDetector::~MotionVectorBasedMotionDetector() = default;

bool MotionVectorBasedMotionDetector::DetectMotion(
    const MotionVectors& motion_vectors) {
  int spatial_grid_number = config_.spatial_grid_number();
  int mv_features_size = spatial_grid_number * spatial_grid_number;

  if (mv_features_spatial_temporal_.size() != mv_features_size) {
    mv_features_spatial_temporal_.resize(mv_features_size, 0.0);
  }

  absl::Status status =
      ::visionai::motion_detection::ComputeMaxMagnitudeEntropyWithSpatialGrid(
          motion_vectors, grids_features_buffer_, spatial_grid_number,
          frame_height_, frame_width_, mv_features_spatial_temporal_);

  if (!status.ok()) {
    LOG(ERROR) << status.message();
  }

  // Only look at the running average mean_motion_magnitude.
  float max_magnitude_entropy = 0;
  int offset = 0;
  for (int i = 0; i < spatial_grid_number; ++i) {
    for (int j = 0; j < spatial_grid_number; ++j) {
      max_magnitude_entropy = std::max(mv_features_spatial_temporal_[offset],
                                       max_magnitude_entropy);
      offset++;
    }
  }

  // if maximum of magnitude entropy is larger than threshold, there is
  // motion in the frame
  return max_magnitude_entropy > config_.motion_sensitivity();
}

std::vector<float> MotionVectorBasedMotionDetector::GetMotionVectorFeatures() {
  if (mv_features_spatial_temporal_.empty()) {
    LOG(WARNING) << "Motion vector feature is empty. Make sure to call "
                 << "DetectMotion first to populate motion vector feature.";
  }
  return mv_features_spatial_temporal_;
}

}  // namespace motion_detection
}  // namespace visionai
