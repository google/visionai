// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/apps/visualization/test_utils.h"

#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/core/mat.hpp"

namespace visionai {
namespace testutils {
bool CheckTwoImagesEqual(const cv::Mat& a, const cv::Mat& b) {
  if ((a.rows != b.rows) || (a.cols != b.cols)) return false;
  cv::Scalar s = cv::sum(a - b);
  cv::Scalar s2 = cv::sum(b - a);
  return (s[0] == 0) && (s[1] == 0) && (s[2] == 0) && (s2[0] == 0) &&
         (s2[1] == 0) && (s2[2] == 0);
}
}  // namespace testutils
}  // namespace visionai
