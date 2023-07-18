// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/visualization/render_utils.h"

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_cat.h"
#include "absl/time/time.h"
#include "visionai/streams/apps/visualization/constants.h"
#include "visionai/util/time_util.h"

namespace visionai {
namespace renderutils {

const cv::Scalar kBlackColor = cv::Scalar(0, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kRedColor = cv::Scalar(0, 0, 255);

cv::Scalar CreateColor(float red, float green, float blue) {
  if (red < 0 || red > 1 || green < 0 || green > 1 || blue < 0 || blue > 1) {
    return kWhiteColor;
  }
  return cv::Scalar(static_cast<int>(blue * 255), static_cast<int>(green * 255),
                    static_cast<int>(red * 255));
}

cv::Scalar GetColorFromPalette(int entry) {
  std::vector<cv::Scalar> palette = {
      CreateColor(/*red=*/1, /*green=*/0, /*blue=*/0),  // Red
      CreateColor(0, 1, 0),                             // Green
      CreateColor(0, 0, 1),                             // Blue
      CreateColor(1, 1, 0),                             // Yellow
      CreateColor(0, 1, 1),                             // Cyan
      CreateColor(1, 0, 1),                             // Purple
      CreateColor(1, 0.5, 0),                           // Orange
      CreateColor(1, 1, 1),                             // White
      CreateColor(0, 0, 0),                             // Black
      CreateColor(0.5, 0.5, 0.5),                       // Gray
      // The following are divided by 2 from the color above.
      CreateColor(0.5, 0, 0), CreateColor(0, 0.5, 0), CreateColor(0, 0, 0.5),
      CreateColor(0.5, 0.5, 0), CreateColor(0, 0.5, 0.5),
      CreateColor(0.5, 0, 0.5), CreateColor(0.5, 0.25, 0),
      CreateColor(0.25, 0.25, 0.25)};
  return palette[entry % palette.size()];
}

void DrawBoundingBoxes(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    bool show_score) {
  for (const auto& box : oc_result.identified_boxes()) {
    const auto& n_box = box.normalized_bounding_box();
    cv::Rect rect(n_box.xmin() * width, n_box.ymin() * height,
                  n_box.width() * width, n_box.height() * height);
    int64_t track_id = box.track_id();
    if (track_id > 0) {
      // If tracking is enabled(line crossing/dwell time), use track_id as the
      // seed to generate a random color for the bounding box.
      cv::Scalar color = GetColorFromPalette(track_id);
      cv::rectangle(img, rect, color, kLineThick2);
      // Put track_id
      double x = n_box.xmin() * width;
      double y = n_box.ymin() * height + n_box.height() * height;
      cv::putText(img, absl::StrCat("-", track_id), cv::Point(x, y - 5),
                  kTextFontFamily, kTrackTextFontscale, color,
                  kStatsTextThickness);
    } else {
      // If tracking is not enabled(full frame count/active zone count without
      // dwelling time), use red color for the bounding box.
      cv::rectangle(img, rect, kRedColor, kLineThick2);
    }
    // Show confidence score.
    if (show_score) {
      double score = box.score();
      // If tracking is enabled, use random color based on track_id. If tracking
      // is disabled, use red color.
      cv::Scalar color = kRedColor;
      if (track_id > 0) {
        color = GetColorFromPalette(track_id);
      }
      double x = n_box.xmin() * width;
      double y = n_box.ymin() * height + n_box.height() * height;
      cv::putText(img, absl::StrCat(score), cv::Point(x, y + 15),
                  kTextFontFamily, kTrackTextFontscale, color,
                  kStatsTextThickness);
    }
  }
}

void DrawBoundingBoxesWithDwellTime(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    absl::flat_hash_map<std::string, absl::Time>& track_id_map,
    absl::flat_hash_map<std::string, std::vector<renderutils::DwellTimeInfo>>&
        dwell_stats_map,
    bool show_score) {
  // Draw bounding boxes and track_ids
  DrawBoundingBoxes(img, width, height, oc_result, show_score);

  // Update track_id_map
  for (const auto& track : oc_result.track_info()) {
    std::string track_id = track.track_id();
    absl::Time update_time = visionai::ToAbseilTimestamp(track.start_time());
    track_id_map[track_id] = update_time;
  }

  // Update dwell_stats_map
  for (const auto& dwell_time : oc_result.dwell_time_info()) {
    std::string track_id = dwell_time.track_id();
    if (dwell_time.dwell_end_time().seconds() == 0) {
      absl::Time dwell_start_time =
          visionai::ToAbseilTimestamp(dwell_time.dwell_start_time());
      // Because this function will be called using the same oc_result protobuf,
      // we want to avoid pushing repeated oc_result dwell time stats.
      if (dwell_stats_map.contains(track_id) &&
          dwell_stats_map[track_id].back().dwell_start_time ==
              dwell_start_time) {
        continue;
      }
      auto dwell_time_info = DwellTimeInfo{
          .zone_id = dwell_time.zone_id(),
          .dwell_start_time = dwell_start_time,
          .dwell_end_time = absl::InfinitePast(),
      };
      dwell_stats_map[track_id].push_back(dwell_time_info);
    } else {
      absl::Time dwell_start_time =
          visionai::ToAbseilTimestamp(dwell_time.dwell_start_time());
      absl::Time dwell_end_time =
          visionai::ToAbseilTimestamp(dwell_time.dwell_end_time());
      // In case with inconsistent data, such as the tool starts somewhere in
      // the middle of a dwelling.
      if (!dwell_stats_map.contains(track_id)) {
        auto dwell_time_info = DwellTimeInfo{
            .zone_id = dwell_time.zone_id(),
            .dwell_start_time = dwell_start_time,
            .dwell_end_time = dwell_end_time,
        };
        dwell_stats_map[track_id].push_back(dwell_time_info);
      } else {
        dwell_stats_map[track_id].back().dwell_start_time = dwell_start_time;
        dwell_stats_map[track_id].back().dwell_end_time = dwell_end_time;
      }
    }
  }

  // Draw dwell stats in bounding boxes
  for (const auto& box : oc_result.identified_boxes()) {
    const std::string track_id = absl::StrCat(box.track_id());
    if (dwell_stats_map.contains(track_id)) {
      DwellTimeInfo last_dwell = dwell_stats_map[track_id].back();
      // Only highlight persons/vehicles which are currently dwelling inside
      // an active zone.
      if (last_dwell.dwell_end_time == absl::InfinitePast()) {
        // Get the color of the active zone and use the same color to draw
        // another rectangle surrounding the bounding box.
        cv::Scalar color = GetColorFromPalette(std::stoi(last_dwell.zone_id));
        const auto& n_box = box.normalized_bounding_box();
        cv::Rect rect(n_box.xmin() * width - 6, n_box.ymin() * height - 6,
                      n_box.width() * width + 12, n_box.height() * height + 12);
        cv::rectangle(img, rect, color, kLineThick2);
      }
    }
  }

  // Cleanup track_id_map and dwell_stats_map.
  // A track_id that doesn't show up for 10 seconds will be removed from both
  // track_id_map and dwell_stats_map, which will help with occlusion issue.
  // An object may disapear in frames and show up later due to occlusion. The
  // model will identify them and assign same track_ids.
  for (auto it = track_id_map.begin(); it != track_id_map.end();) {
    std::string track_id = it->first;
    absl::Time update_time = it->second;
    absl::Time current_time =
        visionai::ToAbseilTimestamp(oc_result.current_time());
    if (current_time - update_time > absl::Seconds(10)) {
      track_id_map.erase(it++);
      if (dwell_stats_map.contains(track_id)) {
        dwell_stats_map.erase(track_id);
      }
    } else {
      ++it;
    }
  }
}

void DrawFullFrameCount(
    cv::Mat img,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha) {
  // TODO(lnli)
  // Now full frame count is empty if no objects are detected.
  // Workaround is to force showing person and vehicle counts.
  // To be updated when annotation is updated.
  int person_count = 0;
  int vehicle_count = 0;
  for (const auto& object_count : oc_result.stats().full_frame_count()) {
    if (object_count.entity().label_string() == "Person") {
      person_count = object_count.count();
    } else if (object_count.entity().label_string() == "Vehicle") {
      vehicle_count = object_count.count();
    }
  }

  // Start drawing the stats table
  int table_width = kFirstColumnWidth + kItemColumnWidth;
  int table_height = 2 * kRowHeight;

  cv::Mat overlay = img.clone();
  cv::Rect black_backgroud(kXInitOffset, kYInitOffset, table_width,
                           table_height);
  cv::rectangle(overlay, black_backgroud, kBlackColor, /* fill */ -1);

  // Draw horizontal lines of the table
  for (int i = 0; i < 3; ++i) {
    cv::line(
        overlay, cv::Point(kXInitOffset, kYInitOffset + i * kRowHeight),
        cv::Point(kXInitOffset + table_width, kYInitOffset + i * kRowHeight),
        kWhiteColor, kLineThick1);
  }

  // Draw vertical lines of the table
  cv::line(overlay, cv::Point(kXInitOffset, kYInitOffset),
           cv::Point(kXInitOffset, kYInitOffset + table_height), kWhiteColor,
           kLineThick1);
  for (int i = 0; i < 2; i++) {
    cv::line(overlay,
             cv::Point(kXInitOffset + kFirstColumnWidth + i * kItemColumnWidth,
                       kYInitOffset),
             cv::Point(kXInitOffset + kFirstColumnWidth + i * kItemColumnWidth,
                       kYInitOffset + table_height),
             kWhiteColor, kLineThick1);
  }

  // Put text
  cv::putText(overlay, "Person",
              cv::Point(kXInitOffset + kXTextOffset,
                        kYInitOffset + kRowHeight + kYTextOffset),
              kTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);
  cv::putText(overlay, absl::StrCat(person_count),
              cv::Point(kXInitOffset + kFirstColumnWidth + kXTextOffset,
                        kYInitOffset + kRowHeight + kYTextOffset),
              kTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);

  cv::putText(overlay, "Vehicle",
              cv::Point(kXInitOffset + kXTextOffset,
                        kYInitOffset + 2 * kRowHeight + kYTextOffset),
              kTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);
  cv::putText(overlay, absl::StrCat(vehicle_count),
              cv::Point(kXInitOffset + kFirstColumnWidth + kXTextOffset,
                        kYInitOffset + 2 * kRowHeight + kYTextOffset),
              kTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);

  // Blend the stats with original image
  cv::addWeighted(overlay, stats_alpha, img, 1 - stats_alpha, 0, img);
}

void DrawActiveZoneCount(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha) {
  // Get all contours
  std::vector<std::vector<cv::Point>> contours;
  for (const auto& zone : oc_result.stats().active_zone_counts()) {
    std::vector<cv::Point> temp_contour;
    for (const auto& vertex :
         zone.annotation().active_zone().normalized_vertices()) {
      temp_contour.push_back(
          cv::Point(vertex.x() * width, vertex.y() * height));
    }
    contours.push_back(std::move(temp_contour));
  }

  // Use random colors to draw active zones
  for (int i = 0; i < contours.size(); ++i) {
    cv::Scalar color = GetColorFromPalette(i);
    cv::drawContours(img, contours, i, color, kLineThick2);
  }

  // Display zone ids next to active zone
  for (int zone_id = 1; zone_id <= contours.size(); ++zone_id) {
    const auto& contour = contours[zone_id - 1];
    cv::putText(img, absl::StrCat("Zone-", zone_id),
                cv::Point(contour.front().x, contour.front().y - 10),
                kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
  }

  // Store "zone_id -> label_string -> count" mapping information
  int number_of_zones = oc_result.stats().active_zone_counts_size();
  absl::flat_hash_map<int, absl::flat_hash_map<std::string, int>> count_map;
  for (int zone_id = 1; zone_id <= number_of_zones; ++zone_id) {
    for (const auto& object_count :
         oc_result.stats().active_zone_counts(zone_id - 1).counts()) {
      int count = object_count.count();
      std::string label = object_count.entity().label_string();
      count_map[zone_id].emplace(label, count);
    }
  }

  // Begin drawing stats information
  cv::Mat overlay = img.clone();

  // Draw black backgroud for stats table
  int table_width = kFirstColumnWidth + kItemColumnWidth * number_of_zones;
  int table_height = kRowHeight * (1 + count_map[1].size());
  cv::Rect black_background(kXInitOffset, kYInitOffset, table_width,
                            table_height);
  cv::rectangle(overlay, black_background, kBlackColor,
                /* fill */ -1);

  // Draw horizontal lines of the stats table
  for (int i = 0; i <= 1 + count_map[1].size(); ++i) {
    cv::line(
        overlay, cv::Point(kXInitOffset, kYInitOffset + i * kRowHeight),
        cv::Point(kXInitOffset + table_width, kYInitOffset + i * kRowHeight),
        kWhiteColor);
  }

  // Draw vertical lines of the stats table
  cv::line(overlay, cv::Point(kXInitOffset, kYInitOffset),
           cv::Point(kXInitOffset, kYInitOffset + table_height), kWhiteColor);
  for (int i = 0; i <= number_of_zones; ++i) {
    cv::line(overlay,
             cv::Point(kXInitOffset + kFirstColumnWidth + i * kItemColumnWidth,
                       kYInitOffset),
             cv::Point(kXInitOffset + kFirstColumnWidth + i * kItemColumnWidth,
                       kYInitOffset + table_height),
             kWhiteColor);
  }

  // Put table header line text
  cv::putText(overlay, "Zone",
              cv::Point(kXInitOffset + kXTextOffset,
                        kYInitOffset + kRowHeight + kYTextOffset),
              kTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);
  for (int zone_id = 1; zone_id <= number_of_zones; ++zone_id) {
    cv::putText(overlay, absl::StrCat(zone_id),
                cv::Point(kXInitOffset + kFirstColumnWidth +
                              (zone_id - 1) * kItemColumnWidth + kXTextOffset,
                          kYInitOffset + kRowHeight + kYTextOffset),
                kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
  }

  // Put person count line
  int y_index = 2;
  if (count_map[1].contains("Person")) {
    cv::putText(overlay, "Person",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int zone_id = 1; zone_id <= number_of_zones; ++zone_id) {
      cv::putText(overlay, absl::StrCat(count_map[zone_id]["Person"]),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (zone_id - 1) * kItemColumnWidth + kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    ++y_index;
  }

  // Put vehicle count line
  if (count_map[1].contains("Vehicle")) {
    cv::putText(overlay, "Vehicle",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int zone_id = 1; zone_id <= number_of_zones; ++zone_id) {
      cv::putText(overlay, absl::StrCat(count_map[zone_id]["Vehicle"]),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (zone_id - 1) * kItemColumnWidth + kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    ++y_index;
  }

  // Blend the stats with original image
  cv::addWeighted(overlay, stats_alpha, img, 1 - stats_alpha, 0, img);
}

void DrawLineCrossingCount(
    cv::Mat img, int width, int height,
    const google::cloud::visionai::v1::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha) {
  int number_of_lines = oc_result.stats().crossing_line_counts_size();

  // Draw crossing lines
  for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
    const auto& crossing_line_count =
        oc_result.stats().crossing_line_counts(line_id - 1);
    int number_of_vertices = crossing_line_count.annotation()
                                 .crossing_line()
                                 .normalized_vertices_size();
    for (int i = 0; i < number_of_vertices - 1; ++i) {
      // Get start point and end point of the line.
      double start_x = crossing_line_count.annotation()
                           .crossing_line()
                           .normalized_vertices(i)
                           .x() *
                       width;

      double start_y = crossing_line_count.annotation()
                           .crossing_line()
                           .normalized_vertices(i)
                           .y() *
                       height;

      double end_x = crossing_line_count.annotation()
                         .crossing_line()
                         .normalized_vertices(i + 1)
                         .x() *
                     width;
      double end_y = crossing_line_count.annotation()
                         .crossing_line()
                         .normalized_vertices(i + 1)
                         .y() *
                     height;

      // Get mid point of the line.
      double mid_x = (start_x + end_x) / 2;
      double mid_y = (start_y + end_y) / 2;

      double k = (start_x - end_x) / (end_y - start_y);

      double arrow_length = std::sqrt(std::pow(end_x - start_x, 2) +
                                      std::pow(end_y - start_y, 2)) /
                            4;
      // Get the start and end point of the arrow.
      double arrow_start_x = mid_x - arrow_length / std::sqrt(1 + k * k);
      double arrow_start_y = mid_y - arrow_length * k / std::sqrt(1 + k * k);
      double arrow_end_x = mid_x + arrow_length / std::sqrt(1 + k * k);
      double arrow_end_y = mid_y + arrow_length * k / std::sqrt(1 + k * k);
      // Change the direction of the arrow if end_y < start_y.
      if (end_y < start_y) {
        double temp_x = arrow_start_x;
        arrow_start_x = arrow_end_x;
        arrow_end_x = temp_x;
        double temp_y = arrow_start_y;
        arrow_start_y = arrow_end_y;
        arrow_end_y = temp_y;
      }
      // Draw the crossing line.
      cv::Scalar color = GetColorFromPalette(line_id - 1);
      cv::line(img, cv::Point(start_x, start_y), cv::Point(end_x, end_y), color,
               kLineThick2);
      // Draw the arrow.
      cv::arrowedLine(img, cv::Point(arrow_start_x, arrow_start_y),
                      cv::Point(arrow_end_x, arrow_end_y), kWhiteColor,
                      kLineThick1);
      // Put mark beside the start of this polyline.
      if (i == 0) {
        cv::putText(img, absl::StrCat("Line-", line_id),
                    cv::Point(start_x + 5, start_y - 5), kTextFontFamily, 1,
                    kWhiteColor, kLineThick1);
      }
    }
  }

  struct LineCounts {
    int positive_count = 0;
    int negative_count = 0;
  };
  // Store "line id -> label -> LineCounts" mapping
  absl::flat_hash_map<int, absl::flat_hash_map<std::string, LineCounts>>
      count_map;
  for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
    const auto& crossing_line_count =
        oc_result.stats().crossing_line_counts(line_id - 1);
    for (const auto& positive_count :
         crossing_line_count.accumulated_positive_direction_counts()) {
      int count = positive_count.object_count().count();
      std::string label = positive_count.object_count().entity().label_string();
      count_map[line_id][label].positive_count = count;
    }
    for (const auto& negative_count :
         crossing_line_count.accumulated_negative_direction_counts()) {
      int count = negative_count.object_count().count();
      std::string label = negative_count.object_count().entity().label_string();
      count_map[line_id][label].negative_count = count;
    }
  }

  // Begin drawing stats information
  cv::Mat overlay = img.clone();

  // Draw black backgroud for stats table
  int table_width =
      kFirstColumnWidth + kItemColumnWidthLineCrossing * number_of_lines;
  int table_height = kRowHeight * (1 + 2 * count_map[1].size());
  cv::Rect black_background(kXInitOffset, kYInitOffset, table_width,
                            table_height);
  cv::rectangle(overlay, black_background, kBlackColor,
                /* fill */ -1);

  // Draw horizontal lines for the stats table
  for (int i = 0; i <= 1 + 2 * count_map[1].size(); ++i) {
    cv::line(
        overlay, cv::Point(kXInitOffset, kYInitOffset + i * kRowHeight),
        cv::Point(kXInitOffset + table_width, kYInitOffset + i * kRowHeight),
        kWhiteColor);
  }

  // Draw vertical lines for the stats table
  cv::line(overlay, cv::Point(kXInitOffset, kYInitOffset),
           cv::Point(kXInitOffset, kYInitOffset + table_height), kWhiteColor);
  for (int i = 0; i <= number_of_lines; ++i) {
    cv::line(overlay,
             cv::Point(kXInitOffset + kFirstColumnWidth +
                           i * kItemColumnWidthLineCrossing,
                       kYInitOffset),
             cv::Point(kXInitOffset + kFirstColumnWidth +
                           i * kItemColumnWidthLineCrossing,
                       kYInitOffset + table_height),
             kWhiteColor);
  }

  // Put table header line text
  cv::putText(overlay, "Line",
              cv::Point(kXInitOffset + kXTextOffset,
                        kYInitOffset + kRowHeight + kYTextOffset),
              kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
              kStatsTextThickness);
  for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
    cv::putText(overlay, absl::StrCat(line_id),
                cv::Point(kXInitOffset + kFirstColumnWidth +
                              (line_id - 1) * kItemColumnWidthLineCrossing +
                              kXTextOffset,
                          kYInitOffset + kRowHeight + kYTextOffset),
                kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                kStatsTextThickness);
  }

  // Put person count line
  int y_index = 2;
  if (count_map[1].contains("Person")) {
    cv::putText(overlay, "Person Enter",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Person"].positive_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    y_index++;
    cv::putText(overlay, "Person Exit",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Person"].negative_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    y_index++;
  }

  // Put vehicle count line
  if (count_map[1].contains("Vehicle")) {
    cv::putText(overlay, "Vehicle Enter",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Vehicle"].positive_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    y_index++;
    cv::putText(overlay, "Vehicle Exit",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Vehicle"].negative_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kTextFontFamily, kStatsTextLineCrossingFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    y_index++;
  }

  // Blend the stats with original image
  cv::addWeighted(overlay, stats_alpha, img, 1 - stats_alpha, 0, img);
}

void ShowAnnotationFps(cv::Mat img, double fps, double stats_alpha) {
  int width = img.cols;

  cv::Mat overlay = img.clone();
  int rect_width = 180;
  int rect_height = 20;
  cv::Rect black_background(width - kXInitOffset - rect_width, kYInitOffset,
                            rect_width, rect_height);
  cv::rectangle(overlay, black_background, kBlackColor, /* fill */ -1);

  std::string fps_str;
  if (fps == 0.0) {
    // Display "-" when still calculating the first fps.
    fps_str = "-";
  } else {
    // Round to 2 digits after decimal point.
    fps_str = absl::StrCat(std::ceil(fps * 100.0) / 100.0);
  }
  cv::putText(overlay, absl::StrCat("Annotation FPS: ", fps_str),
              cv::Point(width - kXInitOffset - rect_width,
                        kYInitOffset + rect_height - 5),
              kTextFontFamily, kTrackTextFontscale, kWhiteColor);

  // Blend the stats with original image
  cv::addWeighted(overlay, stats_alpha, img, 1 - stats_alpha, 0, img);
}
}  // namespace renderutils
}  // namespace visionai
