// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/algorithms/stream_annotation/stream_annotation_util.h"

#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "visionai/algorithms/stream_annotation/geometry_lib.h"
#include "visionai/types/motion_vector.h"
#include "visionai/util/status/status_macros.h"

namespace visionai {
namespace stream_annotation {

using ::testing::FieldsAre;
using ::testing::UnorderedElementsAre;

TEST(StreamAnnotationTest, CanParseEmptyZone) {
  const std::string empty_zone = "";

  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(empty_zone));
  EXPECT_TRUE(zones.empty());
}

TEST(StreamAnnotationTest, CanParseSingleZone) {
  // (1,2) (2,2) (3,2) in one zone.
  const std::string single_zone = "1:2;2:2;3:2";

  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(single_zone));
  EXPECT_EQ(zones.size(), 1);
  EXPECT_THAT(zones.begin()->second,
              UnorderedElementsAre(FieldsAre(/*x=*/1, /*y=*/2),
                                   FieldsAre(/*x=*/2, /*y=*/2),
                                   FieldsAre(/*x=*/3, /*y=*/2)));
}

TEST(StreamAnnotationTest, CanParseMultipleZone) {
  // Two zones in one string input.
  // (1,2) (2,2) (3,2) in one zone.
  // (1,3) (2,3) (3,3) (4,3) in another zone.
  const std::string multiple_zone = "1:2;2:2;3:2-1:3;2:3;3:3;4:3";

  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(multiple_zone));
  EXPECT_EQ(zones.size(), 2);
  EXPECT_THAT(zones.begin()->second,
              UnorderedElementsAre(FieldsAre(/*x=*/1, /*y=*/2),
                                   FieldsAre(/*x=*/2, /*y=*/2),
                                   FieldsAre(/*x=*/3, /*y=*/2)));
  EXPECT_THAT(zones.rbegin()->second,
              UnorderedElementsAre(
                  FieldsAre(/*x=*/1, /*y=*/3), FieldsAre(/*x=*/2, /*y=*/3),
                  FieldsAre(/*x=*/3, /*y=*/3), FieldsAre(/*x=*/4, /*y=*/3)));
}

TEST(StreamAnnotationTest, CanCatchIncompleteZone) {
  // Invalid zone input - only two nodes.
  const std::string incomplete_zone = "1:2;2:2";

  EXPECT_THAT(ParseAnnotationToZoneMap(incomplete_zone),
              testing::status::StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(StreamAnnotationTest, CanCatchIncompleteZones) {
  // Invalid zone input - two zones with one zone incomplete.
  const std::string incomplete_zone = "1:2;2:2;3:2-1:4";

  EXPECT_THAT(ParseAnnotationToZoneMap(incomplete_zone),
              testing::status::StatusIs(absl::StatusCode::kInvalidArgument));
}

TEST(StreamAnnotationTest, CheckPointInZone) {
  // Example zone - (0,0), (0,4), (4,0), (4,4)
  const std::string zone = "0:0;0:4;4:0;4:4";
  const MotionVector src_in_zone{/* .source = */ 1,
                                 /* .w = */ 16,
                                 /* .h = */ 16,
                                 /* .src_x = */ 1,
                                 /* .src_y = */ 1,
                                 /* .dst_x = */ 5,
                                 /* .dst_y = */ 6,
                                 /* .motion_x = */ 20,
                                 /* .motion_y = */ 0,
                                 /* .motion_scale = */ 1};
  const MotionVector dst_in_zone{/* .source = */ 1,
                                 /* .w = */ 16,
                                 /* .h = */ 16,
                                 /* .src_x = */ 6,
                                 /* .src_y = */ 7,
                                 /* .dst_x = */ 2,
                                 /* .dst_y = */ 2,
                                 /* .motion_x = */ 20,
                                 /* .motion_y = */ 0,
                                 /* .motion_scale = */ 1};
  const MotionVector mv_in_zone{/* .source = */ 1,
                                /* .w = */ 16,
                                /* .h = */ 16,
                                /* .src_x = */ 1,
                                /* .src_y = */ 1,
                                /* .dst_x = */ 3,
                                /* .dst_y = */ 3,
                                /* .motion_x = */ 20,
                                /* .motion_y = */ 0,
                                /* .motion_scale = */ 1};
  const MotionVector mv_not_in_zone{/* .source = */ 1,
                                    /* .w = */ 16,
                                    /* .h = */ 16,
                                    /* .src_x = */ 7,
                                    /* .src_y = */ 8,
                                    /* .dst_x = */ 5,
                                    /* .dst_y = */ 6,
                                    /* .motion_x = */ 20,
                                    /* .motion_y = */ 0,
                                    /* .motion_scale = */ 1};
  const MotionVectors input_mv = {src_in_zone, dst_in_zone, mv_in_zone,
                                  mv_not_in_zone};

  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(zone));
  const MotionVectors result_mv =
      MotionVectorsInZone(input_mv, zones, /*exclude_annotation_zone=*/false);

  // Check the number of empty motion vector in list after filtering.
  int non_empty_mv = 0, empty_mv = 0;
  for (int i = 0; i < result_mv.size(); ++i) {
    if (result_mv.at(i).motion_x != 0 || result_mv.at(i).motion_y != 0) {
      non_empty_mv++;
    } else {
      empty_mv++;
    }
  }
  EXPECT_EQ(non_empty_mv, 3);
  EXPECT_EQ(empty_mv, 1);
}

TEST(StreamAnnotationTest, CheckPointOutOfZone) {
  // Example zone - (0,0), (0,4), (4,0), (4,4)
  const std::string zone = "0:0;0:4;4:0;4:4";
  const MotionVector src_in_zone{/* .source = */ 1,
                                 /* .w = */ 16,
                                 /* .h = */ 16,
                                 /* .src_x = */ 1,
                                 /* .src_y = */ 1,
                                 /* .dst_x = */ 5,
                                 /* .dst_y = */ 6,
                                 /* .motion_x = */ 20,
                                 /* .motion_y = */ 1,
                                 /* .motion_scale = */ 1};
  const MotionVector dst_in_zone{/* .source = */ 1,
                                 /* .w = */ 16,
                                 /* .h = */ 16,
                                 /* .src_x = */ 6,
                                 /* .src_y = */ 7,
                                 /* .dst_x = */ 2,
                                 /* .dst_y = */ 2,
                                 /* .motion_x = */ 20,
                                 /* .motion_y = */ 5,
                                 /* .motion_scale = */ 1};
  const MotionVector mv_in_zone{/* .source = */ 1,
                                /* .w = */ 16,
                                /* .h = */ 16,
                                /* .src_x = */ 1,
                                /* .src_y = */ 1,
                                /* .dst_x = */ 3,
                                /* .dst_y = */ 3,
                                /* .motion_x = */ 20,
                                /* .motion_y = */ 10,
                                /* .motion_scale = */ 1};
  const MotionVector mv_not_in_zone{/* .source = */ 1,
                                    /* .w = */ 16,
                                    /* .h = */ 16,
                                    /* .src_x = */ 7,
                                    /* .src_y = */ 8,
                                    /* .dst_x = */ 5,
                                    /* .dst_y = */ 6,
                                    /* .motion_x = */ 20,
                                    /* .motion_y = */ 15,
                                    /* .motion_scale = */ 1};
  const MotionVectors input_mv = {src_in_zone, dst_in_zone, mv_in_zone,
                                  mv_not_in_zone};

  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(zone));
  MotionVectors result_mv =
      MotionVectorsInZone(input_mv, zones, /*exclude_annotation_zone=*/true);

  // Check the number of empty motion vector in list after filtering.
  int non_empty_mv = 0, empty_mv = 0;
  for (int i = 0; i < result_mv.size(); ++i) {
    if (result_mv.at(i).motion_x != 0 || result_mv.at(i).motion_y != 0) {
      non_empty_mv++;
    } else {
      empty_mv++;
    }
  }
  EXPECT_EQ(non_empty_mv, 1);
  EXPECT_EQ(empty_mv, 3);
}

TEST(StreamAnnotationTest, CheckEmptyPointListWithZone) {
  // Example zone - (0,0), (0,4), (4,0), (4,4)
  const std::string zone = "0:0;0:4;4:0;4:4";
  const MotionVectors input_mv = {};

  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(zone));
  MotionVectors result_mv =
      MotionVectorsInZone(input_mv, zones, /*exclude_annotation_zone=*/true);
  EXPECT_EQ(result_mv.size(), 0);
}

TEST(StreamAnnotationTest, CheckEmptyZone) {
  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(""));
  const MotionVector src_in_zone{/* .source = */ 1,
                                 /* .w = */ 16,
                                 /* .h = */ 16,
                                 /* .src_x = */ 1,
                                 /* .src_y = */ 1,
                                 /* .dst_x = */ 5,
                                 /* .dst_y = */ 6,
                                 /* .motion_x = */ 20,
                                 /* .motion_y = */ 1,
                                 /* .motion_scale = */ 1};
  const MotionVector dst_in_zone{/* .source = */ 1,
                                 /* .w = */ 16,
                                 /* .h = */ 16,
                                 /* .src_x = */ 6,
                                 /* .src_y = */ 7,
                                 /* .dst_x = */ 2,
                                 /* .dst_y = */ 2,
                                 /* .motion_x = */ 20,
                                 /* .motion_y = */ 5,
                                 /* .motion_scale = */ 1};
  const MotionVector mv_in_zone{/* .source = */ 1,
                                /* .w = */ 16,
                                /* .h = */ 16,
                                /* .src_x = */ 1,
                                /* .src_y = */ 1,
                                /* .dst_x = */ 3,
                                /* .dst_y = */ 3,
                                /* .motion_x = */ 20,
                                /* .motion_y = */ 10,
                                /* .motion_scale = */ 1};
  const MotionVector mv_not_in_zone{/* .source = */ 1,
                                    /* .w = */ 16,
                                    /* .h = */ 16,
                                    /* .src_x = */ 7,
                                    /* .src_y = */ 8,
                                    /* .dst_x = */ 5,
                                    /* .dst_y = */ 6,
                                    /* .motion_x = */ 20,
                                    /* .motion_y = */ 15,
                                    /* .motion_scale = */ 1};
  const MotionVectors input_mv = {src_in_zone, dst_in_zone, mv_in_zone,
                                  mv_not_in_zone};

  const MotionVectors result_mv =
      MotionVectorsInZone(input_mv, zones, /*exclude_annotation_zone=*/true);
  EXPECT_EQ(result_mv.size(), 4);
}

TEST(StreamAnnotationTest, CheckSinglePointInSingleZone) {
  // Example zone - (0,0), (0,4), (4,0), (4,4)
  const std::string zone = "0:0;0:4;4:0;4:4";
  VAI_ASSERT_OK_AND_ASSIGN(auto zones, ParseAnnotationToZoneMap(zone));

  const Point in_zone{1, 1};      // Points in zone.
  const Point out_of_zone{5, 5};  // Points not in zone.

  EXPECT_TRUE(CheckPointInZone(in_zone, zones.at(0)));
  EXPECT_FALSE(CheckPointInZone(out_of_zone, zones.at(0)));
}

}  // namespace stream_annotation
}  // namespace visionai
