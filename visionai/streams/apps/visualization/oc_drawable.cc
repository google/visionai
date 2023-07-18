#include "visionai/streams/apps/visualization/oc_drawable.h"
#include "visionai/streams/apps/visualization/render_utils.h"

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "visionai/streams/apps/visualization/constants.h"
#include "visionai/streams/apps/visualization/table.h"

namespace visionai {
namespace renderutils {
// Color scalar constants for drawing boxes
const cv::Scalar kBlackColor = cv::Scalar(0, 0, 0);
const cv::Scalar kBlueColor(255, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kRedColor = cv::Scalar(0, 0, 255);
const cv::Scalar kGreenColor(0, 255, 0);

// Draw method that implements the drawable version of the
// Occupancy Analysis Tool
// Will render in bounding boxes around people an vehicles as well as
// different zones and lines
// Also makes use of the Table class to generate statistics for the
// Occupancy Analysis visualization tool
void OccupancyAnalysisDrawable ::draw(cv::Mat& image, int width, int height) {
  // Initialize dwell_stats_map and track_id_map for the dwell time feature.
  // Dwell time feature is assumed to be turned on.
  absl::flat_hash_map<std::string, std::vector<renderutils::DwellTimeInfo>>
        dwell_stats_map = {};
  absl::flat_hash_map<std::string, absl::Time> track_id_map = {};
  bool show_score = true;
  visionai::renderutils::DrawBoundingBoxesWithDwellTime(
      image, width, height, oc_result_, track_id_map, dwell_stats_map,
      show_score);
  if (oc_result_.stats().active_zone_counts().empty() &&
      oc_result_.stats().crossing_line_counts().empty()) {
    DrawFullFrameCount(image, oc_result_);
  } else if (!oc_result_.stats().active_zone_counts().empty()) {
    DrawActiveZoneCount(image, width, height,
                                               oc_result_);
  } else if (!oc_result_.stats().crossing_line_counts().empty()) {
    DrawLineCrossingCount(image, width, height,
                                                 oc_result_);
  }
}

// Method that renders bounding boxes around people and vehicles
// Refactored to be used in the OccupancyAnalaysisDrawable class
// and the new implementation of the Table class
void OccupancyAnalysisDrawable ::DrawFullFrameCount(
    cv::Mat& image,
    const google ::cloud::visionai::v1 ::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha) {
  // Repurposed from render_utils.cc
  int person_count = 0;
  int vehicle_count = 0;
  for (const auto& object_count : oc_result.stats().full_frame_count()) {
    if (object_count.entity().label_string() == "Person") {
      person_count = object_count.count();
    } else if (object_count.entity().label_string() == "Vehicle") {
      vehicle_count = object_count.count();
    }
  }
  // Create the statistics table for the DrawBoundingBoxesWithDwellTime function
  // Will create a table with information on how many boxes have been drawn
  // around people and vehicles
  std::vector<std::vector<std::string>> table_info = {
      {"People", absl::StrCat(person_count)},
      {"Vehicles", absl::StrCat(vehicle_count)}};
  Table table(table_info);
  table.overlay(image, 10, 10);
}

// Method that draws active zones and reports statistics based on these zones
// Refactored to be used in the OccupancyAnalaysisDrawable class
// and the new implementation of the Table class
void OccupancyAnalysisDrawable ::DrawActiveZoneCount(
    cv::Mat& image, int width, int height,
    const google ::cloud::visionai::v1 ::OccupancyCountingPredictionResult&
        oc_result,
    double stats_alpha) {
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
  // Repurposed from render_utils.cc
  // Use random colors to draw active zones
  for (int i = 0; i < contours.size(); ++i) {
    cv::Scalar color = GetColorFromPalette(i);
    cv::drawContours(image, contours, i, color, kLineThick2);
  }
  // Repurposed from render_utils.cc
  // Display zone ids next to active zone
  for (int zone_id = 1; zone_id <= contours.size(); ++zone_id) {
    const auto& contour = contours[zone_id - 1];
    cv::putText(image, absl::StrCat("Zone-", zone_id),
                cv::Point(contour.front().x, contour.front().y - 10),
                kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
  }
  // Repurposed from render_utils.cc
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
  // Create the statistics table for DrawFullFrameCount
  // Table will go through the count_map and create a table containing the
  // number of people and/or vehicles that have passed through a certain zone

  // Create 2d array to parse into the table creator in table.cc
  std::vector<std::vector<std::string>> table_info = {};
  std::vector<std::string> zone_row = {"Zone"};
  // Go through the number of zones and add each one to the table
  for (int zone_id = 1; zone_id < number_of_zones; ++zone_id) {
    zone_row.push_back(absl::StrCat(zone_id - 1));
  }
  table_info.push_back(zone_row);
  // Go through the count_map and check how many people are in each zone
  // and add to the table
  if (count_map[1].contains("Person")) {
    std::vector<std::string> person_row = {"Person"};
    for (int zone_id = 1; zone_id < number_of_zones; ++zone_id) {
      person_row.push_back(absl::StrCat(count_map[zone_id]["Person"]));
    }
    table_info.push_back(person_row);
  }
  // Go through the count_map and check how many vehicles are in each zone
  // and add to the table
  if (count_map[1].contains("Vehicle")) {
    std::vector<std::string> vehicle_row = {"Vehicle"};
    for (int zone_id = 1; zone_id < number_of_zones; ++zone_id) {
      vehicle_row.push_back(absl::StrCat(count_map[zone_id]["Vehicle"]));
    }
    table_info.push_back(vehicle_row);
  }
  Table table(table_info);
  table.overlay(image, 10, 10);
}

// Method that draws lines and reports statistics based on people and vehicles
// entering and exiting these lines
// Refactored to be used in the OccupancyAnalaysisDrawable class
// and the new implementation of the Table class
void OccupancyAnalysisDrawable ::DrawLineCrossingCount(
    cv::Mat& image, int width, int height,
    const google::cloud::visionai::v1 ::OccupancyCountingPredictionResult&
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
      cv::line(image, cv::Point(start_x, start_y), cv::Point(end_x, end_y),
               color, kLineThick2);
      // Draw the arrow.
      cv::arrowedLine(image, cv::Point(arrow_start_x, arrow_start_y),
                      cv::Point(arrow_end_x, arrow_end_y), kWhiteColor,
                      kLineThick1);
      // Put mark beside the start of this polyline.
      if (i == 0) {
        cv::putText(image, absl::StrCat("Line-", line_id),
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
  // Create the statistics table for DrawLineCrossingCount
  // Table will go through the count_map and create a table containing the
  // number of people and/or vehicles that have entered
  // and exited a certain line

  // Create 2d array to parse into the table creator in table.cc
  std::vector<std::vector<std::string>> table_info = {};
  std::vector<std::string> zone_row = {"Line"};
  for (int zone_id = 1; zone_id < number_of_lines; ++zone_id) {
    zone_row.push_back(absl::StrCat(zone_id - 1));
  }
  table_info.push_back(zone_row);
  if (count_map[1].contains("Person")) {
    std::vector<std::string> person_enter_row = {"Person Enter"};
    std::vector<std::string> person_exit_row = {"Person Exit"};
    for (int zone_id = 1; zone_id < number_of_lines; ++zone_id) {
      person_enter_row.push_back(
          absl::StrCat(count_map[zone_id]["Person"].positive_count));
      person_exit_row.push_back(
          absl::StrCat(count_map[zone_id]["Person"].negative_count));
    }
    table_info.push_back(person_enter_row);
    table_info.push_back(person_exit_row);
  }
  if (count_map[1].contains("Vehicle")) {
    std::vector<std::string> vehicle_enter_row = {"Vehicle Enter"};
    std::vector<std::string> vehicle_exit_row = {"Vehicle Exit"};
    for (int zone_id = 1; zone_id < number_of_lines; ++zone_id) {
      vehicle_enter_row.push_back(
          absl::StrCat(count_map[zone_id]["Vehicle"].positive_count));
      vehicle_exit_row.push_back(
          absl::StrCat(count_map[zone_id]["Vehicle"].negative_count));
    }
    table_info.push_back(vehicle_enter_row);
    table_info.push_back(vehicle_exit_row);
  }
  Table table(table_info);
  table.overlay(image, 10, 10);
}
}  // namespace renderutils
}  // namespace visionai

