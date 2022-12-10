// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/line_crossing_util.h"

#include "absl/strings/str_split.h"

namespace visionai {
namespace util {

std::vector<std::vector<std::string>> ParsePolyLineSegments(
    std::string line_segment_end_points) {
  std::vector<std::string> line_groups =
      absl::StrSplit(line_segment_end_points, '-');
  std::vector<std::vector<std::string>> end_points_group;
  // if there is no '-' in the line_segment_end_points, only 1 polyline
  // provided, create a single end point group
  if (line_groups.empty()) {
    end_points_group.push_back(absl::StrSplit(line_segment_end_points, ';'));
  } else {
    for (int i = 0; i < line_groups.size(); ++i) {
      end_points_group.push_back(absl::StrSplit(line_groups[i], ';'));
    }
  }
  return end_points_group;
}

}  // namespace util
}  // namespace visionai
