// Copyright 2022 Google LLC
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "visionai/streams/apps/visualization/render_utils.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "google/protobuf/timestamp.pb.h"
#include "gtest/gtest.h"
#include "opencv4/opencv2/core.hpp"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "absl/container/flat_hash_map.h"
#include "absl/strings/str_cat.h"
#include "absl/time/time.h"
#include "visionai/util/time_util.h"

namespace visionai {
namespace {
using ::google::cloud::visionai::v1::OccupancyCountingPredictionResult;

constexpr int kRowHeight = 40;
constexpr int kXInitOffset = 10;
constexpr int kYInitOffset = 20;
constexpr int kLineThick1 = 1;
constexpr int kLineThick2 = 2;
constexpr int kFirstColumnWidth = 160;
constexpr int kItemColumnWidth = 90;
constexpr int kItemColumnWidthLineCrossing = 160;
constexpr int kStatsTextFontFamily = cv::FONT_HERSHEY_DUPLEX;
constexpr double kStatsTextFontscale = 1;
constexpr double kTrackTextFontscale = 0.5;
constexpr double kStatsTextLineCrossingFontscale = 0.6;
constexpr double kStatsTextThickness = 1;
constexpr int kXTextOffset = 20;
constexpr int kYTextOffset = -10;
const cv::Scalar kBlackColor = cv::Scalar(0, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kRedColor = cv::Scalar(0, 0, 255);

// Checks if two RGB images are exactly equal.
bool CheckTwoImagesEqual(const cv::Mat a, const cv::Mat b) {
  if ((a.rows != b.rows) || (a.cols != b.cols)) return false;
  cv::Scalar s = sum(a - b);
  return (s[0] == 0) && (s[1] == 0) && (s[2] == 0);
}

// Tests if CreateColor works as expected.
TEST(CreateColor, TestCreateColor) {
  EXPECT_EQ(kWhiteColor, renderutils::CreateColor(2, 0, 0));
  EXPECT_EQ(kWhiteColor, renderutils::CreateColor(-1, 0, 0));
  EXPECT_EQ(kWhiteColor, renderutils::CreateColor(0, 2, 0));
  EXPECT_EQ(kWhiteColor, renderutils::CreateColor(0, -1, 0));
  EXPECT_EQ(kWhiteColor, renderutils::CreateColor(0, 0, 2));
  EXPECT_EQ(kWhiteColor, renderutils::CreateColor(0, 0, -1));
  EXPECT_EQ(cv::Scalar(127, 127, 127), renderutils::CreateColor(0.5, 0.5, 0.5));
}

// Tests if GetColorFromPalette works as expected.
TEST(GetColorFromPalette, TestGetColorFromPalette) {
  EXPECT_EQ(kRedColor, renderutils::GetColorFromPalette(0));
  EXPECT_EQ(kRedColor, renderutils::GetColorFromPalette(18));
}

// Tests if DrawBoundingBoxes works as expected.
TEST(DrawBoundingBoxes, TestOneBoundingBox) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* box = oc_result.add_identified_boxes();
  box->mutable_normalized_bounding_box()->set_xmin(0);
  box->mutable_normalized_bounding_box()->set_ymin(0);
  box->mutable_normalized_bounding_box()->set_width(1);
  box->mutable_normalized_bounding_box()->set_height(0.5);

  // Prepare expected output frame.
  const uint8_t pixels[10 * 10] = {
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  const cv::Mat1b expected_frame =
      cv::Mat1b(10, 10, const_cast<uint8_t*>(pixels));

  cv::Mat atom_img(10, 10, CV_8UC3, cv::Scalar(0, 0, 0));

  // Call DrawBoundingBoxes
  renderutils::DrawBoundingBoxes(atom_img, 10, 10, oc_result);

  // First convert output image to grey scale, then to binary image
  cv::cvtColor(atom_img, atom_img, cv::COLOR_BGR2GRAY);
  cv::threshold(atom_img, atom_img, 0, 1, cv::THRESH_BINARY);

  // Get the difference matrix
  const cv::Mat difference_mat = atom_img != expected_frame;
  cv::threshold(difference_mat, difference_mat, 0, 1, cv::THRESH_BINARY);
  EXPECT_EQ(0, cv::countNonZero(difference_mat));
}

// Tests if DrawBoundingBoxes works with show_score turned on
TEST(DrawBoundingBoxes, TestOneBoundingBoxWithShowScore) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* box = oc_result.add_identified_boxes();
  box->mutable_normalized_bounding_box()->set_xmin(0.5);
  box->mutable_normalized_bounding_box()->set_ymin(0.5);
  box->mutable_normalized_bounding_box()->set_width(0.1);
  box->mutable_normalized_bounding_box()->set_height(0.15);
  box->set_score(0.58);

  // Call DrawBoundingBoxes with a black image.
  cv::Mat atom_img(600, 800, CV_8UC3, kBlackColor);
  renderutils::DrawBoundingBoxes(atom_img, 800, 600, oc_result, true);

  // Prepare expected_img.
  cv::Mat expected_img(600, 800, CV_8UC3, kBlackColor);

  const auto& n_box = box->normalized_bounding_box();
  cv::Rect rect(n_box.xmin() * 800, n_box.ymin() * 600, n_box.width() * 800,
                n_box.height() * 600);
  cv::rectangle(expected_img, rect, kRedColor, kLineThick2);

  double score = box->score();
  cv::Scalar color = kRedColor;
  double x = n_box.xmin() * 800;
  double y = n_box.ymin() * 600 + n_box.height() * 600;
  cv::putText(expected_img, absl::StrCat(score), cv::Point(x, y + 15),
              kStatsTextFontFamily, kTrackTextFontscale, color,
              kStatsTextThickness);

  EXPECT_TRUE(CheckTwoImagesEqual(atom_img, expected_img));
}

// Tests if DrawBoundingBoxes works with show_score and track_id
TEST(DrawBoundingBoxes, TestOneBoundingBoxWithShowScoreTrackId) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* box = oc_result.add_identified_boxes();
  box->mutable_normalized_bounding_box()->set_xmin(0.5);
  box->mutable_normalized_bounding_box()->set_ymin(0.5);
  box->mutable_normalized_bounding_box()->set_width(0.1);
  box->mutable_normalized_bounding_box()->set_height(0.15);
  box->set_score(0.58);
  box->set_track_id(100);

  // Call DrawBoundingBoxes with a black image.
  cv::Mat atom_img(600, 800, CV_8UC3, kBlackColor);
  renderutils::DrawBoundingBoxes(atom_img, 800, 600, oc_result, true);

  // Prepare expected_img.
  cv::Mat expected_img(600, 800, CV_8UC3, kBlackColor);

  const auto& n_box = box->normalized_bounding_box();
  cv::Rect rect(n_box.xmin() * 800, n_box.ymin() * 600, n_box.width() * 800,
                n_box.height() * 600);
  cv::Scalar color = renderutils::GetColorFromPalette(100);
  cv::rectangle(expected_img, rect, color, kLineThick2);
  // Put track_id
  double x = n_box.xmin() * 800;
  double y = n_box.ymin() * 600 + n_box.height() * 600;
  cv::putText(expected_img, absl::StrCat("-", 100), cv::Point(x, y - 5),
              kStatsTextFontFamily, kTrackTextFontscale, color,
              kStatsTextThickness);

  // Show confidence score.
  double score = box->score();
  cv::putText(expected_img, absl::StrCat(score), cv::Point(x, y + 15),
              kStatsTextFontFamily, kTrackTextFontscale, color,
              kStatsTextThickness);

  EXPECT_TRUE(CheckTwoImagesEqual(atom_img, expected_img));
}

// Tests if DrawBoundingBoxesWithDwellTime works as expected.
TEST(DrawBoundingBoxesWithDwellTime, OneDwellTime) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* box = oc_result.add_identified_boxes();
  box->mutable_normalized_bounding_box()->set_xmin(0.5);
  box->mutable_normalized_bounding_box()->set_ymin(0.5);
  box->mutable_normalized_bounding_box()->set_width(0.2);
  box->mutable_normalized_bounding_box()->set_height(0.2);
  box->set_track_id(100);
  box->set_score(0.6);

  auto* track_info = oc_result.add_track_info();
  track_info->set_track_id(absl::StrCat(100));
  track_info->mutable_start_time()->set_seconds(1666221820);
  track_info->mutable_start_time()->set_nanos(738322438);

  auto* dwell_time_info = oc_result.add_dwell_time_info();
  dwell_time_info->set_track_id(absl::StrCat(100));
  dwell_time_info->mutable_dwell_start_time()->set_seconds(1666221814);
  dwell_time_info->mutable_dwell_start_time()->set_nanos(738322438);
  dwell_time_info->set_zone_id(absl::StrCat(0));

  absl::flat_hash_map<std::string, absl::Time> track_id_map;

  absl::flat_hash_map<std::string, std::vector<renderutils::DwellTimeInfo>>
      dwell_stats_map;

  // Call DrawBoundingBoxesWithDwellTime with a black image.
  cv::Mat atom_img(600, 800, CV_8UC3, kBlackColor);
  renderutils::DrawBoundingBoxesWithDwellTime(
      atom_img, 800, 600, oc_result, track_id_map, dwell_stats_map, true);

  // Prepare the expected_img.
  cv::Mat expected_img(600, 800, CV_8UC3, kBlackColor);

  // Draw bounding boxes and track_ids
  renderutils::DrawBoundingBoxes(expected_img, 800, 600, oc_result, true);

  // Draw dwell stats in bounding boxes
  for (const auto& box : oc_result.identified_boxes()) {
    cv::Scalar color = kRedColor;
    const auto& n_box = box.normalized_bounding_box();
    cv::Rect rect(n_box.xmin() * 800 - 6, n_box.ymin() * 600 - 6,
                  n_box.width() * 800 + 12, n_box.height() * 600 + 12);
    cv::rectangle(expected_img, rect, color, kLineThick2);
  }

  EXPECT_TRUE(CheckTwoImagesEqual(atom_img, expected_img));
}

// Tests DrawFullFrameCount function.
TEST(DrawFullFrameCount, PersonAndVehicle) {
  // Prepare the oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* person_count_obj =
      oc_result.mutable_stats()->mutable_full_frame_count()->Add();
  person_count_obj->mutable_entity()->set_label_string("Person");
  person_count_obj->set_count(6);
  auto* vehicle_count_obj =
      oc_result.mutable_stats()->mutable_full_frame_count()->Add();
  vehicle_count_obj->mutable_entity()->set_label_string("Vehicle");
  vehicle_count_obj->set_count(3);

  // Call DrawFullFrameCount with a black image.
  cv::Mat atom_img(600, 800, CV_8UC3, kRedColor);
  renderutils::DrawFullFrameCount(atom_img, oc_result);

  // Prepare expected output image.
  cv::Mat expected_img(600, 800, CV_8UC3, kRedColor);

  int person_count = 6;
  int vehicle_count = 3;

  // Start drawing the stats table
  int table_width = kFirstColumnWidth + kItemColumnWidth;
  int table_height = 2 * kRowHeight;

  cv::Mat overlay = expected_img.clone();
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
              kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);
  cv::putText(overlay, absl::StrCat(person_count),
              cv::Point(kXInitOffset + kFirstColumnWidth + kXTextOffset,
                        kYInitOffset + kRowHeight + kYTextOffset),
              kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);

  cv::putText(overlay, "Vehicle",
              cv::Point(kXInitOffset + kXTextOffset,
                        kYInitOffset + 2 * kRowHeight + kYTextOffset),
              kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);
  cv::putText(overlay, absl::StrCat(vehicle_count),
              cv::Point(kXInitOffset + kFirstColumnWidth + kXTextOffset,
                        kYInitOffset + 2 * kRowHeight + kYTextOffset),
              kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);

  // Blend the stats with original image
  cv::addWeighted(overlay, 0.5, expected_img, 0.5, 0, expected_img);

  EXPECT_TRUE(CheckTwoImagesEqual(atom_img, expected_img));
}

// Tests DrawActiveZoneCount function.
// Only tests if active zones are drawn, ignore stats table right now.
// I will come back if I have good ideas.
TEST(DrawActiveZoneCount, OneActiveZone) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* active_zone_count = oc_result.mutable_stats()->add_active_zone_counts();
  auto* vertex_1 = active_zone_count->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_1->set_x(0);
  vertex_1->set_y(0);
  auto* vertex_2 = active_zone_count->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_2->set_x(0);
  vertex_2->set_y(0.5);
  auto* vertex_3 = active_zone_count->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_3->set_x(0.5);
  vertex_3->set_y(0.5);
  auto* vertex_4 = active_zone_count->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_4->set_x(0.5);
  vertex_4->set_y(0);

  // Prepare expected output frame.
  const uint8_t pixels[10 * 10] = {
      1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
      1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0,
      1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  const cv::Mat1b expected_frame =
      cv::Mat1b(10, 10, const_cast<uint8_t*>(pixels));

  cv::Mat atom_img(10, 10, CV_8UC3, kBlackColor);

  // Call DrawActiveZoneCount
  renderutils::DrawActiveZoneCount(atom_img, 10, 10, oc_result);

  // First convert output image to grey scale, then to binary image
  cv::cvtColor(atom_img, atom_img, cv::COLOR_BGR2GRAY);
  cv::threshold(atom_img, atom_img, 0, 1, cv::THRESH_BINARY);

  // Get the difference matrix
  const cv::Mat difference_mat = atom_img != expected_frame;
  cv::threshold(difference_mat, difference_mat, 0, 1, cv::THRESH_BINARY);
  EXPECT_EQ(0, cv::countNonZero(difference_mat));
}

// Tests if stats table is correctly drawn for the active zone count scenario.
TEST(DrawActiveZoneCount, StatsTable) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* active_zone_count_1 =
      oc_result.mutable_stats()->add_active_zone_counts();
  auto* person_1 = active_zone_count_1->add_counts();
  person_1->set_count(5);
  person_1->mutable_entity()->set_label_string("Person");
  auto* vehicle_1 = active_zone_count_1->add_counts();
  vehicle_1->set_count(2);
  vehicle_1->mutable_entity()->set_label_string("Vehicle");
  auto* active_zone_count_2 =
      oc_result.mutable_stats()->add_active_zone_counts();
  auto* person_2 = active_zone_count_2->add_counts();
  person_2->set_count(4);
  person_2->mutable_entity()->set_label_string("Person");
  auto* vehicle_2 = active_zone_count_2->add_counts();
  vehicle_2->set_count(3);
  vehicle_2->mutable_entity()->set_label_string("Vehicle");

  auto* vertex_1 = active_zone_count_1->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_1->set_x(0);
  vertex_1->set_y(0);
  auto* vertex_2 = active_zone_count_1->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_2->set_x(0);
  vertex_2->set_y(0.5);
  auto* vertex_3 = active_zone_count_1->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_3->set_x(0.5);
  vertex_3->set_y(0.5);
  auto* vertex_4 = active_zone_count_1->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_4->set_x(0.5);
  vertex_4->set_y(0);

  auto* vertex_5 = active_zone_count_2->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_5->set_x(0.5);
  vertex_5->set_y(0.5);
  auto* vertex_6 = active_zone_count_2->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_6->set_x(0.5);
  vertex_6->set_y(1);
  auto* vertex_7 = active_zone_count_2->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_7->set_x(1);
  vertex_7->set_y(1);
  auto* vertex_8 = active_zone_count_2->mutable_annotation()
                       ->mutable_active_zone()
                       ->add_normalized_vertices();
  vertex_8->set_x(1);
  vertex_8->set_y(0.5);

  absl::flat_hash_map<int, absl::flat_hash_map<std::string, int>> count_map;
  count_map[1].emplace("Person", 5);
  count_map[1].emplace("Vehicle", 2);
  count_map[2].emplace("Person", 4);
  count_map[2].emplace("Vehicle", 3);

  // Call DrawActiveZoneCount with a black image.
  cv::Mat atom_img(600, 800, CV_8UC3, kBlackColor);
  renderutils::DrawActiveZoneCount(atom_img, 800, 600, oc_result);

  // Prepare expected_image.
  cv::Mat expected_img(600, 800, CV_8UC3, kBlackColor);

  // Get all contours
  std::vector<std::vector<cv::Point>> contours;
  for (const auto& zone : oc_result.stats().active_zone_counts()) {
    std::vector<cv::Point> temp_contour;
    for (const auto& vertex :
         zone.annotation().active_zone().normalized_vertices()) {
      temp_contour.push_back(cv::Point(vertex.x() * 800, vertex.y() * 600));
    }
    contours.push_back(std::move(temp_contour));
  }

  // Use random colors to draw active zones
  for (int i = 0; i < contours.size(); ++i) {
    cv::Scalar color = renderutils::GetColorFromPalette(i);
    cv::drawContours(expected_img, contours, i, color, kLineThick2);
  }

  // Display zone ids next to active zone
  for (int zone_id = 1; zone_id <= contours.size(); ++zone_id) {
    const auto& contour = contours[zone_id - 1];
    cv::putText(expected_img, absl::StrCat("Zone-", zone_id),
                cv::Point(contour.front().x, contour.front().y - 10),
                kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
  }

  int number_of_zones = 2;
  // Begin drawing stats information
  cv::Mat overlay = expected_img.clone();

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
              kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
              kStatsTextThickness);
  for (int zone_id = 1; zone_id <= number_of_zones; ++zone_id) {
    cv::putText(overlay, absl::StrCat(zone_id),
                cv::Point(kXInitOffset + kFirstColumnWidth +
                              (zone_id - 1) * kItemColumnWidth + kXTextOffset,
                          kYInitOffset + kRowHeight + kYTextOffset),
                kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
  }

  // Put person count line
  int y_index = 2;
  if (count_map[1].contains("Person")) {
    cv::putText(overlay, "Person",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int zone_id = 1; zone_id <= number_of_zones; ++zone_id) {
      cv::putText(overlay, absl::StrCat(count_map[zone_id]["Person"]),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (zone_id - 1) * kItemColumnWidth + kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    ++y_index;
  }

  // Put vehicle count line
  if (count_map[1].contains("Vehicle")) {
    cv::putText(overlay, "Vehicle",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
    for (int zone_id = 1; zone_id <= number_of_zones; ++zone_id) {
      cv::putText(overlay, absl::StrCat(count_map[zone_id]["Vehicle"]),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (zone_id - 1) * kItemColumnWidth + kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kStatsTextFontFamily, kStatsTextFontscale, kWhiteColor,
                  kStatsTextThickness);
    }
    ++y_index;
  }

  // Blend the stats with original image
  cv::addWeighted(overlay, 0.5, expected_img, 0.5, 0, expected_img);

  EXPECT_TRUE(CheckTwoImagesEqual(atom_img, expected_img));
}

// Tests if lines are drawn correctly on the frame.
TEST(DrawLineCrossingCount, TwoLines) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* crossing_line_count_1 =
      oc_result.mutable_stats()->mutable_crossing_line_counts()->Add();
  auto* vertex_1 = crossing_line_count_1->mutable_annotation()
                       ->mutable_crossing_line()
                       ->add_normalized_vertices();
  vertex_1->set_x(0);
  vertex_1->set_y(0);
  auto* vertex_2 = crossing_line_count_1->mutable_annotation()
                       ->mutable_crossing_line()
                       ->add_normalized_vertices();
  vertex_2->set_x(1);
  vertex_2->set_y(1);
  auto* crossing_line_count_2 =
      oc_result.mutable_stats()->mutable_crossing_line_counts()->Add();
  auto* vertex_3 = crossing_line_count_2->mutable_annotation()
                       ->mutable_crossing_line()
                       ->add_normalized_vertices();
  vertex_3->set_x(0);
  vertex_3->set_y(1);
  auto* vertex_4 = crossing_line_count_2->mutable_annotation()
                       ->mutable_crossing_line()
                       ->add_normalized_vertices();
  vertex_4->set_x(1);
  vertex_4->set_y(0);

  // Prepare expected output frame.
  const uint8_t pixels[10 * 10] = {
      1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1,
      1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1,
      0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1};

  const cv::Mat1b expected_frame =
      cv::Mat1b(10, 10, const_cast<uint8_t*>(pixels));

  cv::Mat atom_img(10, 10, CV_8UC3, kBlackColor);

  // Call DrawLineCrossingCount
  renderutils::DrawLineCrossingCount(atom_img, 10, 10, oc_result);

  // First convert output image to grey scale, then to binary image
  cv::cvtColor(atom_img, atom_img, cv::COLOR_BGR2GRAY);
  cv::threshold(atom_img, atom_img, 0, 1, cv::THRESH_BINARY);

  // Get the difference matrix
  const cv::Mat difference_mat = atom_img != expected_frame;
  cv::threshold(difference_mat, difference_mat, 0, 1, cv::THRESH_BINARY);
  EXPECT_EQ(0, cv::countNonZero(difference_mat));
}

// Tests if stats table are drawn correctly in line crossing count.
TEST(DrawLineCrossingCount, StatsTable) {
  // Prepare oc_result protobuf.
  OccupancyCountingPredictionResult oc_result;
  auto* crossing_line_count =
      oc_result.mutable_stats()->add_crossing_line_counts();
  auto* vertex_1 = crossing_line_count->mutable_annotation()
                       ->mutable_crossing_line()
                       ->add_normalized_vertices();
  vertex_1->set_x(0);
  vertex_1->set_y(0);
  auto* vertex_2 = crossing_line_count->mutable_annotation()
                       ->mutable_crossing_line()
                       ->add_normalized_vertices();
  vertex_2->set_x(1);
  vertex_2->set_y(1);
  auto* positive_count =
      crossing_line_count->add_accumulated_positive_direction_counts();
  auto* negative_count =
      crossing_line_count->add_accumulated_negative_direction_counts();
  positive_count->mutable_object_count()->mutable_entity()->set_label_string(
      "Person");
  positive_count->mutable_object_count()->set_count(100);
  negative_count->mutable_object_count()->mutable_entity()->set_label_string(
      "Person");
  negative_count->mutable_object_count()->set_count(60);

  // Get the actual image output
  cv::Mat actual_image(600, 800, CV_8UC3, kBlackColor);
  renderutils::DrawLineCrossingCount(actual_image, 800, 600, oc_result);

  // Prepare the expected_image
  cv::Mat expected_image(600, 800, CV_8UC3, kBlackColor);
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
                       800;

      double start_y = crossing_line_count.annotation()
                           .crossing_line()
                           .normalized_vertices(i)
                           .y() *
                       600;

      double end_x = crossing_line_count.annotation()
                         .crossing_line()
                         .normalized_vertices(i + 1)
                         .x() *
                     800;
      double end_y = crossing_line_count.annotation()
                         .crossing_line()
                         .normalized_vertices(i + 1)
                         .y() *
                     600;

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
      cv::Scalar color = renderutils::GetColorFromPalette(line_id - 1);
      cv::line(expected_image, cv::Point(start_x, start_y),
               cv::Point(end_x, end_y), color, kLineThick2);
      // Draw the arrow.
      cv::arrowedLine(expected_image, cv::Point(arrow_start_x, arrow_start_y),
                      cv::Point(arrow_end_x, arrow_end_y), kWhiteColor,
                      kLineThick1);
      // Put mark beside the start of this polyline.
      if (i == 0) {
        cv::putText(expected_image, absl::StrCat("Line-", line_id),
                    cv::Point(start_x + 5, start_y - 5), kStatsTextFontFamily,
                    1, kWhiteColor, kLineThick1);
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
  cv::Mat overlay = expected_image.clone();

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
              kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
              kWhiteColor, kStatsTextThickness);
  for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
    cv::putText(overlay, absl::StrCat(line_id),
                cv::Point(kXInitOffset + kFirstColumnWidth +
                              (line_id - 1) * kItemColumnWidthLineCrossing +
                              kXTextOffset,
                          kYInitOffset + kRowHeight + kYTextOffset),
                kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                kWhiteColor, kStatsTextThickness);
  }

  // Put person count line
  int y_index = 2;
  if (count_map[1].contains("Person")) {
    cv::putText(overlay, "Person Enter",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                kWhiteColor, kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Person"].positive_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                  kWhiteColor, kStatsTextThickness);
    }
    y_index++;
    cv::putText(overlay, "Person Exit",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                kWhiteColor, kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Person"].negative_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                  kWhiteColor, kStatsTextThickness);
    }
    y_index++;
  }

  // Put vehicle count line
  if (count_map[1].contains("Vehicle")) {
    cv::putText(overlay, "Vehicle Enter",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                kWhiteColor, kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Vehicle"].positive_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                  kWhiteColor, kStatsTextThickness);
    }
    y_index++;
    cv::putText(overlay, "Vehicle Exit",
                cv::Point(kXInitOffset + kXTextOffset,
                          kYInitOffset + kRowHeight * y_index + kYTextOffset),
                kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                kWhiteColor, kStatsTextThickness);
    for (int line_id = 1; line_id <= number_of_lines; ++line_id) {
      cv::putText(overlay,
                  absl::StrCat(count_map[line_id]["Vehicle"].negative_count),
                  cv::Point(kXInitOffset + kFirstColumnWidth +
                                (line_id - 1) * kItemColumnWidthLineCrossing +
                                kXTextOffset,
                            kYInitOffset + kRowHeight * y_index + kYTextOffset),
                  kStatsTextFontFamily, kStatsTextLineCrossingFontscale,
                  kWhiteColor, kStatsTextThickness);
    }
    y_index++;
  }

  // Blend the stats with original image
  cv::addWeighted(overlay, 0.5, expected_image, 0.5, 0, expected_image);

  EXPECT_TRUE(CheckTwoImagesEqual(actual_image, expected_image));
}

// Tests if ShowAnnotationFps works correct.
TEST(ShowAnnotationFps, ShowAnnotationFps) {
  // Call DrawBoundingBoxes with a black image.
  cv::Mat atom_img(600, 800, CV_8UC3, kBlackColor);
  renderutils::ShowAnnotationFps(atom_img, 2.78, 1.0);

  // Prepare expected_img.
  cv::Mat expected_img(600, 800, CV_8UC3, kBlackColor);

  int rect_width = 180;
  int rect_height = 20;
  cv::putText(expected_img, absl::StrCat("Annotation FPS: ", 2.78),
              cv::Point(800 - kXInitOffset - rect_width,
                        kYInitOffset + rect_height - 5),
              kStatsTextFontFamily, kTrackTextFontscale, kWhiteColor);

  EXPECT_TRUE(CheckTwoImagesEqual(atom_img, expected_img));
}

// Tests if cleaning maps in DrawBoundingBoxesWithDwellTime works correct.
TEST(DrawBoundingBoxesWithDwellTime, CleanMaps) {
  // track_id_map stores "track_id -> latest_update_time" mapping.
  absl::flat_hash_map<std::string, absl::Time> track_id_map;
  // dwell_stats_map stores "track_id -> a vector of DwellTimeInfo" mapping
  absl::flat_hash_map<std::string, std::vector<renderutils::DwellTimeInfo>>
      dwell_stats_map;

  absl::Time a_special_time;
  std::string error;
  if (absl::ParseTime(absl::RFC3339_full, "2020-01-01T00:00:00.000000000+00:00",
                      &a_special_time, &error)) {
    track_id_map["1"] = a_special_time;
    dwell_stats_map["1"].push_back(renderutils::DwellTimeInfo());

    track_id_map["2"] = a_special_time + absl::Seconds(15);
    dwell_stats_map["2"].push_back(renderutils::DwellTimeInfo());

    // Prepare oc_result protobuf.
    OccupancyCountingPredictionResult oc_result;
    ::google::protobuf::Timestamp current_time =
        visionai::ToProtoTimestamp(a_special_time + absl::Seconds(20));
    oc_result.mutable_current_time()->set_seconds(current_time.seconds());

    // We don't really care about the drawing
    cv::Mat actual_image(600, 800, CV_8UC3, kBlackColor);
    renderutils::DrawBoundingBoxesWithDwellTime(
        actual_image, 800, 600, oc_result, track_id_map, dwell_stats_map);

    // track_id=1 record should be removed and track_id=2 record should remain.
    EXPECT_FALSE(track_id_map.contains("1"));
    EXPECT_TRUE(track_id_map.contains("2"));

    EXPECT_FALSE(dwell_stats_map.contains("1"));
    EXPECT_TRUE(dwell_stats_map.contains("2"));
  }
}
}  // namespace

}  // namespace visionai
