#include "visionai/algorithms/stream_annotation/geometry_lib.h"

#include "absl/status/status.h"
#include "absl/strings/str_format.h"

namespace visionai {
namespace stream_annotation {

absl::StatusOr<bool> PointInPolygon(const Point& point, const Polygon& polygon,
                                    const float epsilon) {
  const int size = polygon.size();
  if (size < 3) {
    return absl::InvalidArgumentError(
        absl::StrFormat("A polygon must have at least 3 points, but this "
                        "polygon only has %d points.",
                        size));
  }

  // Adapted from
  // https://source.corp.google.com/piper///depot/google3/third_party/OpenCVX/v3_4_0/modules/imgproc/src/geometry.cpp;l=162-189;rcl=529254800.
  int count = 0;
  for (int i = 0; i < size; i++) {
    const Point& prev = polygon[(i + size - 1) % size];
    const Point& curr = polygon[i];

    if ((prev.y < (point.y + epsilon) && curr.y < (point.y + epsilon)) ||
        (prev.y > (point.y - epsilon) && curr.y > (point.y - epsilon)) ||
        (prev.x < (point.x + epsilon) && curr.x < (point.x + epsilon))) {
      if (abs(point.y - curr.y) < epsilon && abs(point.x - curr.x) < epsilon) {
        return true;
      }
      if (abs(point.y - curr.y) < epsilon && abs(point.y - prev.y) < epsilon &&
          ((prev.x < (point.x + epsilon) && point.x < (curr.x + epsilon)) ||
           (curr.x < (point.x + epsilon) && point.x < (prev.x + epsilon)))) {
        return true;
      }
      continue;
    }

    double dist = (point.y - prev.y) * (curr.x - prev.x) -
                  (point.x - prev.x) * (curr.y - prev.y);
    if (curr.y < prev.y) dist = -dist;

    if (dist == 0.0f) return true;
    if (dist > 0.0f) count++;
  }

  return count % 2 == 1;
}

}  // namespace stream_annotation
}  // namespace visionai
