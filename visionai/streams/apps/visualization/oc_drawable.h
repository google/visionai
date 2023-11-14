/*
 * Copyright (c) 2023 Google LLC All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_OC_DRAWABLE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_OC_DRAWABLE_H_

#include <string>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "visionai/streams/apps/visualization/drawable.h"
#include "visionai/streams/apps/visualization/render_utils.h"

// Drawable subclass for the Occupancy Analysis model
namespace visionai {
namespace renderutils {


class OccupancyAnalysisDrawable: public Drawable {
 public:
  // Constructs a new instance of the OccupancyAnalysisDrawable with a
  // oc_result passed into it. The constructor will move the oc_result parameter
  // to the OCAnalysisDrawable object
  explicit OccupancyAnalysisDrawable(
      google::cloud::visionai::v1::OccupancyCountingPredictionResult& oc_result)
      : oc_result_(std::move(oc_result)) {}
  struct DwellTimeInfo {
    std::string zone_id;
    absl::Time dwell_start_time;
    absl::Time dwell_end_time;
   };
   
   void draw(cv::Mat& image, int width, int height);

  private:
   google::cloud::visionai::v1::OccupancyCountingPredictionResult oc_result_;
   double annotation_fps = 0.0;
   
   void DrawFullFrameCount(
       cv::Mat& image,
       const google ::cloud::visionai::v1 ::OccupancyCountingPredictionResult&
           oc_result,
       double stats_alpha = 0.5);
   
   void DrawActiveZoneCount(
       cv::Mat& image, int width, int height,
       const google ::cloud::visionai::v1 ::OccupancyCountingPredictionResult&
           oc_result,
       double stats_alpha = 0.5);
   
   void DrawLineCrossingCount(
       cv::Mat& image, int width, int height,
       const google::cloud::visionai::v1 ::OccupancyCountingPredictionResult&
           oc_result,
       double stats_alpha = 0.5);
};
}  // namespace renderutils
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_OC_DRAWABLE_H_
