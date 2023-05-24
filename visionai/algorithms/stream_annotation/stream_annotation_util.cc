// Copyright 2023 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/algorithms/stream_annotation/stream_annotation_util.h"

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include "glog/logging.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_split.h"
#include "visionai/algorithms/stream_annotation/geometry_lib.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/line_crossing_util.h"

namespace visionai {
namespace stream_annotation {
namespace {

using ::visionai::util::ParsePolyLineSegments;

}  // namespace

absl::StatusOr<std::map<uint64_t, Polygon>> ParseAnnotationToZoneMap(
    const std::string zone_annotation) {
  std::map<uint64_t, Polygon> zone_map;

  // No zone annotation, return empty map.
  if (zone_annotation.empty()) {
    LOG(WARNING) << "Zone annotation is empty.";
    return zone_map;
  }

  std::vector<std::vector<std::string>> end_points_group =
      ParsePolyLineSegments(zone_annotation);

  for (int j = 0; j < end_points_group.size(); ++j) {
    for (int i = 0; i < end_points_group[j].size(); ++i) {
      float x, y;
      if (end_points_group[j].size() < 3) {
        return absl::InvalidArgumentError("At least 3 vertices are required.");
      }
      std::vector<std::string> end_points =
          absl::StrSplit(end_points_group[j][i], ':');
      if (!absl::SimpleAtof(end_points[0], &x)) {
        return absl::InvalidArgumentError(
            absl::StrFormat("X is %s", end_points[0]));
      }
      if (!absl::SimpleAtof(end_points[1], &y)) {
        return absl::InvalidArgumentError(
            absl::StrFormat("Y is %s", end_points[1]));
      }

      zone_map[j].push_back(Point{x, y});
    }
  }
  return zone_map;
}

bool CheckPointInZone(const Point& point, const Polygon& zone) {
  if (zone.empty()) {
    LOG(WARNING) << "Zone is empty.";
    return false;
  }

  const auto is_in_zone = PointInPolygon(point, zone);
  if (!is_in_zone.ok()) {
    LOG(WARNING) << is_in_zone.status();
    return false;
  }
  return *is_in_zone;
}

MotionVectors MotionVectorsInZone(const MotionVectors& motion_vectors,
                                  std::map<uint64_t, Polygon>& zone_map,
                                  bool exclude_annotation_zone) {
  // No zone map, return original list.
  if (zone_map.empty()) {
    LOG(WARNING) << "Zone map is empty.";
    return motion_vectors;
  }

  MotionVectors filtered_motion_vectors;
  filtered_motion_vectors.reserve(motion_vectors.size());

  for (auto& mv : motion_vectors) {
    Point src{static_cast<float>(mv.src_x), static_cast<float>(mv.src_y)};
    Point dst{static_cast<float>(mv.dst_x), static_cast<float>(mv.dst_y)};

    bool in_zone = false;
    // Check if source or destination points of the motion vector are in any
    // zone from the zone map.
    for (auto& [zone_id, zone] : zone_map) {
      if (CheckPointInZone(src, zone) || CheckPointInZone(dst, zone)) {
        in_zone = true;
        break;
      }
    }

    filtered_motion_vectors.push_back(mv);

    // If mv is in the annotation zone while excluding the annotation zone OR
    // mv is not in the annotation zone while including the annotation zone,
    // set the motion vector to zero magnitude to remove it from motion feature
    // max magnitude calculation.
    if ((exclude_annotation_zone && in_zone) ||
        (!exclude_annotation_zone && !in_zone)) {
      filtered_motion_vectors.back().motion_x = 0;
      filtered_motion_vectors.back().motion_y = 0;
    }
  }
  return filtered_motion_vectors;
}

MotionVectors RemoveEmptyMotionVector(MotionVectors filtered_motion_vectors_) {
  MotionVectors nonempty_motion_vector;
  nonempty_motion_vector.reserve(filtered_motion_vectors_.size());
  for (int i = 0; i < filtered_motion_vectors_.size(); ++i) {
    if (filtered_motion_vectors_.at(i).motion_x != 0 ||
        filtered_motion_vectors_.at(i).motion_y != 0) {
      nonempty_motion_vector.push_back(filtered_motion_vectors_.at(i));
    }
  }
  return nonempty_motion_vector;
}

}  // namespace stream_annotation
}  // namespace visionai
