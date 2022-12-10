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

#include "visionai/algorithms/detection/motion_detection/motion_vector_based_motion_detector_config.pb.h"
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
  std::vector<float> GetMotionVectorFeatures();

 private:
  MotionVectorBasedMotionDetectorConfig config_;

  int frame_width_, frame_height_;

  // Holds the spatial temporal motion feature buffer, which gets continuously
  // updated  when new motion vectors come in.
  gtl::CircularBuffer<Array3D<float>> grids_features_buffer_;

  // Final motion features used for motion prediction in all the spatial grids.
  std::vector<float> mv_features_spatial_temporal_;
};

}  // namespace motion_detection
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_DETECTION_MOTION_DETECTION_MOTION_VECTOR_BASED_MOTION_DETECTOR_H_
