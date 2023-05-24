// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_RENDER_UTILS_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_RENDER_UTILS_H_

#include <string>
#include <utility>
#include <vector>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "absl/container/flat_hash_map.h"

namespace visionai {
namespace renderutils {

// Features dwell time information. Gives information about zone id, start time
// and end time of a dwelling instance. If the dwelling has not ended,
// dwell_end_time will be set as absl::InfinitePast().
struct DwellTimeInfo {
  std::string zone_id;
  absl::Time dwell_start_time;
  absl::Time dwell_end_time;
};

// Creates a color according to propositions of each channel(Red, Green, Blue).
cv::Scalar CreateColor(float red, float green, float blue);

// Returns a color at a specific entry from the palette.
cv::Scalar GetColorFromPalette(int entry);

// Draws bounding boxes according to identified boxes in Occupancy Counter
// prediction result protobuf
void DrawBoundingBoxes(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    bool show_score = false);

// Draws bounding boxes with dwell time according to identified boxes in
// Occupancy Counter prediction result protobuf.
void DrawBoundingBoxesWithDwellTime(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    absl::flat_hash_map<std::string, absl::Time>& track_id_map,
    absl::flat_hash_map<std::string, std::vector<DwellTimeInfo>>&
        dwell_stats_map,
    bool show_score = false);

// Draws full frame count stats based on Occupancy Counter prediction result
// protobuf
void DrawFullFrameCount(
    cv::Mat img,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha = 0.5);

// Draws active zone count stats based on Occupancy Counter prediction result
// protobuf
void DrawActiveZoneCount(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha = 0.5);

// Draws line crossing count stats based on Occupancy Counter prediction result
// protobuf
void DrawLineCrossingCount(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha = 0.5);

// Shows fps of annotation output stream on the frame
void ShowAnnotationFps(cv::Mat img, double fps, double stats_alpha = 0.5);

}  // namespace renderutils
}  // namespace visionai

#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_RENDER_UTILS_H_
