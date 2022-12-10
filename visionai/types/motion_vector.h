// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_TYPES_MOTION_VECTOR_H_
#define THIRD_PARTY_VISIONAI_TYPES_MOTION_VECTOR_H_

#include <cstdint>
#include <vector>

namespace visionai {

// A struct to represent motion vector.
struct MotionVector {
  // Note where the current microblock comes from.
  // Negative value means it comes from the past.
  // Positive value means it comes from the future.
  int source = 0;

  // Shape of the block.
  uint8_t w = 0;
  uint8_t h = 0;

  // Absolute source pixel position. Could be out of frame.
  int16_t src_x = 0;
  int16_t src_y = 0;

  // Absolute destination pixel position. Could be out of frame.
  int16_t dst_x = 0;
  int16_t dst_y = 0;

  // src_x = dst_x + motion_x / motion_scale
  // src_y = dst_y + motion_y / motion_scale
  int motion_x = 0;
  int motion_y = 0;
  uint16_t motion_scale = 0;
};

using MotionVectors = std::vector<MotionVector>;

}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_TYPES_MOTION_VECTOR_H_
