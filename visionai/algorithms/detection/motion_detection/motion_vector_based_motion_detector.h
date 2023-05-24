/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_MOTION_VECTOR_BASED_MOTION_DETECTOR_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_MOTION_VECTOR_BASED_MOTION_DETECTOR_H_

// Determine if motion is presented in a frame by analyzing spatial temporal
// motion vector features.

#include <vector>

#include "absl/status/statusor.h"
#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector_config.pb.h"
#include "visionai/algorithms/stream_annotation/geometry_lib.h"
#include "visionai/algorithms/stream_annotation/stream_annotation_util.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/array/array3d.h"
#include "visionai/util/gtl/circularbuffer.h"

namespace visionai {
namespace motion_detection {

class MotionVectorBasedMotionDetector {
 public:
  explicit MotionVectorBasedMotionDetector(
      const MotionVectorBasedMotionDetectorConfig& config, int width,
      int height);
  virtual ~MotionVectorBasedMotionDetector();

  // Perform motion detection by calculating features from motion vector in the
  // specific frame.
  bool DetectMotion(const MotionVectors& motion_vectors);

  // Performs motion detection with regard to the annotated zones.
  // Accepts a vector of motion vectors and a zone configuration.
  // If the zone config contains no zone, detect motion in the whole frame.
  // Else, detects motion only within the zones specified in the zone config.
  // Default in zone config is to use annotated zones as regions of interest and
  // only detect motion in the annotated zones.
  // Set exclude_annotated_zone to TRUE to detect motion outside of the zones.
  absl::StatusOr<bool> ZoneBasedDetectMotion(
      const MotionVectors& motion_vectors,
      MotionVectorBasedMotionDetectorZoneConfig zone_config);

  // Return motion vector features based on spatial grid.
  std::vector<float> GetMotionVectorFeatures();

  // Return filtered motion vector based on zone config.
  std::vector<MotionVector> GetFilteredMotionVector();

  // Return zone map.
  std::map<uint64_t, stream_annotation::Polygon> GetZoneMap();

 private:
  MotionVectorBasedMotionDetectorConfig config_;

  int frame_width_, frame_height_;

  // Holds the spatial temporal motion feature buffer, which gets continuously
  // updated  when new motion vectors come in.
  gtl::CircularBuffer<Array3D<float>> grids_features_buffer_;

  // Motion features used for motion prediction in all the spatial grids.
  std::vector<float> mv_features_spatial_temporal_;

  // Filtered motion vector based on zone config.
  std::vector<MotionVector> filtered_motion_vectors_;

  // Zone map from zone annotation.
  std::map<uint64_t, stream_annotation::Polygon> zone_map_;
};

}  // namespace motion_detection
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_MOTION_VECTOR_BASED_MOTION_DETECTOR_H_
