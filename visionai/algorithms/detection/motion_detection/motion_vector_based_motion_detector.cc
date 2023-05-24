// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector.h"

#include <stdint.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/algorithms/detection/motion_detection/util.h"
#include "visionai/algorithms/stream_annotation/geometry_lib.h"
#include "visionai/algorithms/stream_annotation/stream_annotation_util.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/array/array3d.h"
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

  // If maximum of magnitude entropy is larger than threshold, there is
  // motion in the frame.
  return max_magnitude_entropy > config_.motion_sensitivity();
}

absl::StatusOr<bool> MotionVectorBasedMotionDetector::ZoneBasedDetectMotion(
    const MotionVectors& motion_vectors,
    MotionVectorBasedMotionDetectorZoneConfig zone_config) {
  filtered_motion_vectors_.clear();

  if (!zone_config.zone_annotation().empty()) {
    VAI_ASSIGN_OR_RETURN(zone_map_,
                     ::visionai::stream_annotation::ParseAnnotationToZoneMap(
                         zone_config.zone_annotation()));
    filtered_motion_vectors_ =
        ::visionai::stream_annotation::MotionVectorsInZone(
            motion_vectors, zone_map_, zone_config.exclude_annotated_zone());
  } else {
    filtered_motion_vectors_ = motion_vectors;
  }

  return DetectMotion(filtered_motion_vectors_);
}

std::vector<float> MotionVectorBasedMotionDetector::GetMotionVectorFeatures() {
  if (mv_features_spatial_temporal_.empty()) {
    LOG(WARNING) << "Motion vector feature is empty. Make sure to call "
                 << "DetectMotion first to populate motion vector feature.";
  }
  return mv_features_spatial_temporal_;
}

std::vector<MotionVector>
MotionVectorBasedMotionDetector::GetFilteredMotionVector() {
  if (filtered_motion_vectors_.empty()) {
    LOG(WARNING) << "Filtered motion vector is empty. Make sure to call "
                 << "ZoneBasedDetectMotion first to populate filtered "
                 << "motion vector.";
  }

  return ::visionai::stream_annotation::RemoveEmptyMotionVector(
      filtered_motion_vectors_);
}

std::map<uint64_t, stream_annotation::Polygon>
MotionVectorBasedMotionDetector::GetZoneMap() {
  return zone_map_;
}

}  // namespace motion_detection
}  // namespace visionai
