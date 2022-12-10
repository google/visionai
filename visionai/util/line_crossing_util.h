/*
 * Copyright 2022 Google LLC
 *
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file or at
 * https://developers.google.com/open-source/licenses/bsd
 */

#ifndef VISIONAI_UTIL_LINE_CROSSING_UTIL_H_
#define VISIONAI_UTIL_LINE_CROSSING_UTIL_H_

#include <vector>

#include "absl/strings/string_view.h"

namespace visionai {
namespace util {

// Parse polyline segments text x1:y1;x2:y2;x3,y3-x4:y4;x5:y5 into vector of x:y
std::vector<std::vector<std::string>> ParsePolyLineSegments(
    std::string line_segment_end_points);

}  // namespace util
}  // namespace visionai

#endif  // VISIONAI_UTIL_LINE_CROSSING_UTIL_H_
