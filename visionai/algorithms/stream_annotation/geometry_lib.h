/*
 * Copyright (c) 2023 Google LLC All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_STREAM_ANNOTATION_GEOMETRY_LIB_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_STREAM_ANNOTATION_GEOMETRY_LIB_H_

#include <vector>

#include "absl/status/statusor.h"

namespace visionai {
namespace stream_annotation {

struct Point {
  float x = 0.0f;
  float y = 0.0f;
};

typedef std::vector<Point> Polygon;

// Checks if a point is in a polygon.
absl::StatusOr<bool> PointInPolygon(const Point& point, const Polygon& polygon,
                                    float epsilon = 1e-8f);

}  // namespace stream_annotation
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_STREAM_ANNOTATION_GEOMETRY_LIB_H_
