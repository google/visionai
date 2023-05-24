#include "visionai/algorithms/stream_annotation/geometry_lib.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "absl/status/status.h"
#include "visionai/testing/status/status_matchers.h"

namespace visionai {
namespace stream_annotation {
namespace {

using ::testing::HasSubstr;

TEST(PointInPolygon, TooFewPoints) {
  EXPECT_THAT(PointInPolygon(Point{.x = 0.5f, .y = 0.5f},
                             Polygon{Point{0.2f, 0.2f}, Point{0.8f, 0.8f}}),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       HasSubstr("A polygon must have at least 3 points")));
}

TEST(PointInPolygon, BasicTrue) {
  EXPECT_THAT(PointInPolygon(Point{.x = 0.5f, .y = 0.5f},
                             Polygon{{0.2f, 0.2f}, {0.8f, 0.2f}, {0.5f, 0.8f}}),
              IsOkAndHolds(true));
}

TEST(PointInPolygon, BasicFalse) {
  EXPECT_THAT(PointInPolygon(Point{.x = 0.8f, .y = 0.5f},
                             Polygon{{0.2f, 0.2f}, {0.8f, 0.2f}, {0.5f, 0.8f}}),
              IsOkAndHolds(false));
}

TEST(PointInPolygon, OnEdge) {
  EXPECT_THAT(PointInPolygon(Point{.x = 0.5f, .y = 0.2f},
                             Polygon{{0.2f, 0.2f}, {0.8f, 0.2f}, {0.5f, 0.8f}}),
              IsOkAndHolds(true));
}

TEST(PointInPolygon, AtVertex) {
  EXPECT_THAT(PointInPolygon(Point{.x = 0.2f, .y = 0.2f},
                             Polygon{{0.2f, 0.2f}, {0.8f, 0.2f}, {0.5f, 0.8f}}),
              IsOkAndHolds(true));
}

TEST(PointInPolygon, ConcavePolygonTrue) {
  EXPECT_THAT(
      PointInPolygon(
          Point{.x = 0.5f, .y = 0.6f},
          Polygon{{0.2f, 0.2f}, {0.5f, 0.4f}, {0.8f, 0.2f}, {0.5f, 0.8f}}),
      IsOkAndHolds(true));
}

TEST(PointInPolygon, ConcavePolygonFalse) {
  EXPECT_THAT(
      PointInPolygon(
          Point{.x = 0.5f, .y = 0.3f},
          Polygon{{0.2f, 0.2f}, {0.5f, 0.4f}, {0.8f, 0.2f}, {0.5f, 0.8f}}),
      IsOkAndHolds(false));
}

}  // namespace
}  // namespace stream_annotation
}  // namespace visionai
