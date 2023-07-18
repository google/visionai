#include "visionai/streams/apps/visualization/oc_drawable.h"

#include <string>
#include <vector> 

#include "gtest/gtest.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "visionai/streams/apps/visualization/constants.h"
#include "visionai/streams/apps/visualization/table.h"
#include "visionai/streams/apps/visualization/test_utils.h"



// Tests for the Occupancy Analysis Drawable class

namespace visionai {
namespace renderutils {

const cv::Scalar kBlackColor = cv::Scalar(0, 0, 0);
const cv::Scalar kBlueColor(255, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kRedColor = cv::Scalar(0, 0, 255);
const cv::Scalar kGreenColor(0, 255, 0);

// If you provide a source with no detection info make sure the draw contains
// something different than the blank Mat. This is because even with empty
// detection the table should still be drawn.

using google::cloud::visionai::v1::OccupancyCountingPredictionResult;

TEST(OccupancyAnalysisDrawable, TestNofInfoHasTable) {
  cv::Mat base_mat(mat_rows, mat_cols, CV_8UC3, kGreenColor);
  cv::Mat expected_mat = base_mat.clone();
  
  OccupancyCountingPredictionResult oc_result;
  OccupancyAnalysisDrawable oc_drawable(oc_result);
  
  
  oc_drawable.draw(base_mat, base_mat.size().width, base_mat.size().height);
  
  EXPECT_FALSE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
  
}

// Validate that a table is drawn with the correct categories even if there
// is nothing to detect
TEST(OccupancyAnalysisDrawable, OnlyBoundingBoxes) {
  
  cv::Mat base_mat(mat_rows, mat_cols, CV_8UC3, kGreenColor);
  cv::Mat expected_mat = base_mat.clone();
  

  OccupancyCountingPredictionResult oc_result;
  
  OccupancyAnalysisDrawable oc_drawable(oc_result);
  oc_drawable.draw(base_mat, base_mat.size().width, base_mat.size().height);
  
  std::vector<std::vector<std::string>> table_info{{"People", "0"},
                                                  {"Vehicles", "0"}};
  Table table(table_info);
  table.overlay(expected_mat, 10, 10);
  
  if (oc_result.stats().active_zone_counts().empty()
     && oc_result.stats().crossing_line_counts().empty()) {
      EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
  } else {
      EXPECT_FALSE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
  }
  
}

// Validate that if there exits a person or a vehicle the program
// will correctly identify them, draw bounding boxes around them, and
// render in a table with the proper count of each identified person/vehicle

TEST(OccupancyAnalysisDrawable, IdentifyPeopleAndVehicles) {
  OccupancyCountingPredictionResult oc_result;
  OccupancyAnalysisDrawable oc_drawable(oc_result);
  auto* person_count_obj =
      oc_result.mutable_stats()->mutable_full_frame_count()->Add();
  person_count_obj->mutable_entity()->set_label_string("Person");
  person_count_obj->set_count(5);
  auto* vehicle_count_obj =
      oc_result.mutable_stats()->mutable_full_frame_count()->Add();
  vehicle_count_obj->mutable_entity()->set_label_string("Vehicle");
  vehicle_count_obj->set_count(3);
  
  // Call DrawFullFrameCount with a black image.
  cv::Mat atom_img(mat_cols, mat_rows, CV_8UC3, kRedColor);
  oc_drawable.draw(atom_img, atom_img.size().width, atom_img.size().height);
  cv::Mat expected_img(mat_cols, mat_rows, CV_8UC3, kRedColor);
  
  int person_count = 5;
  int vehicle_count = 3;
  
  std::vector<std::vector<std::string>> table_info = {
      {"People", absl::StrCat(person_count)},
      {"Vehicles", absl::StrCat(vehicle_count)}};
  Table table(table_info);
  table.overlay(expected_img, 10, 10);
  
  if (!oc_result.stats().active_zone_counts().empty()
      && oc_result.stats().crossing_line_counts().empty()) {
    EXPECT_TRUE(testutils::CheckTwoImagesEqual(atom_img, expected_img));
  } else {
    EXPECT_FALSE(testutils::CheckTwoImagesEqual(atom_img, expected_img));
  }
}
// Test if the Occupancy Analytics Drawable correctly identifies people and
// vehicles in active zones and creates a statistics table with the data
// TODO: Retest once proper flow is set up for more accurate testing.
TEST(OccupancyAnalysisDrawable, IdentifyActiveZones) {
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

  // Create OccupancyAnalysisDrawable object
  // Call the draw method to render in the zones
  cv::Mat atom_img(mat_cols, mat_rows, CV_8UC3, kBlackColor);
  OccupancyAnalysisDrawable oc_drawable(oc_result);
  oc_drawable.draw(atom_img, mat_rows, mat_cols);

  // Prepare expected_image.
  cv::Mat expected_img(mat_cols, mat_rows, CV_8UC3, kBlackColor);

  // Get all contours
  std::vector<std::vector<cv::Point>> contours;
  for (const auto& zone : oc_result.stats().active_zone_counts()) {
    std::vector<cv::Point> temp_contour;
    for (const auto& vertex :
         zone.annotation().active_zone().normalized_vertices()) {
      temp_contour.push_back(cv::Point(vertex.x() * mat_rows, 
                                       vertex.y() * mat_cols));
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
                kTextFontFamily, kStatsTextFontscale, kWhiteColor,
                kStatsTextThickness);
  }

  int number_of_zones = 2;
  
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
  table.overlay(expected_img, 10, 10);
  if (!oc_result.stats().active_zone_counts().empty()
      && oc_result.stats().crossing_line_counts().empty()) {
    EXPECT_TRUE(testutils::CheckTwoImagesEqual(atom_img, expected_img));
  } else {
    EXPECT_FALSE(testutils::CheckTwoImagesEqual(atom_img, expected_img));
  }

}

}  // namespace renderutils
}  // namespace visionai
