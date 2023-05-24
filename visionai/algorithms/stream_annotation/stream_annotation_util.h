// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_ALGORITHMS_STREAM_ANNOTATION_STREAM_ANNOTATIONS_UTIL_H_
#define THIRD_PARTY_VISIONAI_ALGORITHMS_STREAM_ANNOTATION_STREAM_ANNOTATIONS_UTIL_H_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "absl/status/statusor.h"
#include "visionai/algorithms/stream_annotation/geometry_lib.h"
#include "visionai/types/motion_vector.h"

namespace visionai {
namespace stream_annotation {

// Parse string input of stream annotation into a map of zone with nodes in each
// zones. Each zone must have three or more nodes to be valid.
// Use : to connect x and y axis. Use ; to connect nodes within same zone.
// Use - to connect multiple zones.
// Example input: 1:1;1:2;1:3-2:1;2:2;2:3;2:4
// Example output: A map of two zones.
//                 Zone one has node (1,1), (1,2), (1,3).
//                 Zone two has node (2,1), (2,2), (2,3), (2,4).
absl::StatusOr<std::map<uint64_t, Polygon>> ParseAnnotationToZoneMap(
    std::string zone_annotation);

// Filter input motion vectors based on the zones. If a motion vector is in the
// area we want to exclude, the magnitude of the motion vector will be set to
// zero. The exclude_annotation_zone marks if we want to exclude motion vectors
// in the passed in zones.
// If exclude_annotation_zone is true, the motion vectors in the zone will have
// zero magnitude upon return.
// If exclude_annotation_zone is false, the motion vectors not in the zone will
// have zero magnitude upon return.
MotionVectors MotionVectorsInZone(const MotionVectors& motion_vectors,
                                  std::map<uint64_t, Polygon>& zone_map,
                                  bool exclude_annotation_zone);

// Check if a point is within the zone. Return true if it is.
bool CheckPointInZone(const Point& point, const Polygon& zone);

// Returns a list of motion vector that do not have zero magnitude.
MotionVectors RemoveEmptyMotionVector(MotionVectors filtered_motion_vectors_);

}  // namespace stream_annotation
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_ALGORITHMS_STREAM_ANNOTATION_STREAM_ANNOTATIONS_UTIL_H_
