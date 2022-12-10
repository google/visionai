// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/util/line_crossing_util.h"

#include "gtest/gtest.h"

namespace visionai {
namespace util {
namespace {

// Test for parsing polyline segments text to vector x:y text.
TEST(LineCrossingUtilTest, ParseSinglePolyLineSegments) {
  std::string line_segment_end_points =
      "0.14:0.6;0.61:0.62";
  std::vector<std::vector<std::string>> exp_line_segments = {
      {"0.14:0.6", "0.61:0.62"}};
  std::vector<std::vector<std::string>> end_points_group =
      ParsePolyLineSegments(line_segment_end_points);
  ASSERT_EQ(end_points_group.size(), 1);
  for (int j = 0; j < end_points_group.size(); ++j) {
    for (int i = 0; i < end_points_group[j].size(); ++i) {
      ASSERT_EQ(exp_line_segments[j][i], end_points_group[j][i]);
    }
  }
}

// Test for parsing polyline segments text to vector x:y text.
TEST(LineCrossingUtilTest, ParseMultiplePolyLineSegments) {
  std::string line_segment_end_points =
      "0.14:0.6;0.61:0.62-0.24:0.36;0.51:0.32";
  std::vector<std::vector<std::string>> exp_line_segments = {
      {"0.14:0.6", "0.61:0.62"}, {"0.24:0.36", "0.51:0.32"}};
  std::vector<std::vector<std::string>> end_points_group =
      ParsePolyLineSegments(line_segment_end_points);
  ASSERT_EQ(end_points_group.size(), 2);
  for (int j = 0; j < end_points_group.size(); ++j) {
    for (int i = 0; i < end_points_group[j].size(); ++i) {
      ASSERT_EQ(exp_line_segments[j][i], end_points_group[j][i]);
    }
  }
}

}  // namespace
}  // namespace util
}  // namespace visionai
