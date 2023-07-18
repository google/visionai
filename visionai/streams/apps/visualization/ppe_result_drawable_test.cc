#include "visionai/streams/apps/visualization/ppe_result_drawable.h"

#include <string>
#include <vector>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "gtest/gtest.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "visionai/streams/apps/visualization/constants.h"
#include "visionai/streams/apps/visualization/table.h"
#include "visionai/streams/apps/visualization/test_utils.h"

namespace visionai {
namespace {

// Some declarations for simplicity and to keep naming concise when creating the
// test output objects
using google::cloud::visionai::v1::PersonalProtectiveEquipmentDetectionOutput;
using PersonIdentifiedBox = google::cloud::visionai::v1::
    PersonalProtectiveEquipmentDetectionOutput_PersonIdentifiedBox;
using NormalBoundingBox = google::cloud::visionai::v1::
    PersonalProtectiveEquipmentDetectionOutput_NormalizedBoundingBox;
using DetectedPerson = google::cloud::visionai::v1::
    PersonalProtectiveEquipmentDetectionOutput_DetectedPerson;
using IdentifiedPPEBox = google::cloud::visionai::v1::
    PersonalProtectiveEquipmentDetectionOutput_PPEIdentifiedBox;

// Color scalars for drawing
const cv::Scalar kGreenColor(0, 255, 0);
const cv::Scalar kBlueColor(255, 0, 0);
const cv::Scalar kRedColor(0, 0, 255);

const cv::Scalar kWhiteColor(255, 255, 255);

// Adds a PPE entity and bounding box for the detected person. Used to generate
// the test objects for tests including checking the correctness of PPE bounding
// boxes and labels
void AddPPEBox(DetectedPerson* person) {
  IdentifiedPPEBox* ppe_box = person->add_detected_ppe_identified_boxes();
  NormalBoundingBox* bounding_box = ppe_box->mutable_normalized_bounding_box();
  bounding_box->set_xmin(0.5);
  bounding_box->set_ymin(0.3);
  bounding_box->set_width(0.1);
  bounding_box->set_height(0.1);

  ppe_box->set_confidence_score(0.33);

  ppe_box->mutable_ppe_entity()->set_ppe_label_string("Helmet");
}

// Adds a Person bounding box for the detected person. Used to generate test
// objects for tests including checking the correctness of Person bounding boxes
// and labels
void AddPersonBox(DetectedPerson* person) {
  auto* person_box = person->mutable_detected_person_identified_box()
                         ->mutable_normalized_bounding_box();
  person_box->set_xmin(0.4);
  person_box->set_width(0.2);
  person_box->set_ymin(0.5);
  person_box->set_height(0.4);

  person->mutable_detected_person_identified_box()->set_confidence_score(0.5);
}

// helper method to draw text in the same place it should appear in the actual
// drawing. This draws centered under the bounding box with a padding as
// specified in the constants file
void DrawText(cv::Mat& image, std::string text, cv::Rect rect) {
  cv::Size size = cv::getTextSize(text, renderutils::kTextFontFamily,
                                  renderutils::kTrackTextFontscale,
                                  renderutils::kStatsTextThickness, 0);
  int x = (rect.tl().x + rect.br().x) / 2 - size.width / 2;
  int y = rect.br().y + renderutils::kLabelTextPaddingY + size.height;

  cv::putText(image, text, cv::Point(x, y), renderutils::kTextFontFamily,
              renderutils::kTrackTextFontscale, kWhiteColor,
              renderutils::kStatsTextThickness);
}

// If we provide a source with no detection info, make sure the drawn contains
// something different than the blank Mat. This is because an empty detection
// should still draw the table.
TEST(PPEResultDrawable, TestNoDataHasTable) {
  cv::Mat base_mat(800, 600, CV_8UC3, kGreenColor);

  cv::Mat expected_mat = base_mat.clone();

  PersonalProtectiveEquipmentDetectionOutput output;
  renderutils::PPEResultDrawable drawable(output);
  drawable.draw(base_mat, base_mat.size().width, base_mat.size().height);

  EXPECT_FALSE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// If we have a source with no detection info, validate that only the table is
// drawn with both fields set to 0
TEST(PPEResultDrawable, TestNoDataTableContent) {
  cv::Mat base_mat(800, 600, CV_8UC3, kGreenColor);

  cv::Mat expected_mat = base_mat.clone();

  PersonalProtectiveEquipmentDetectionOutput output;
  renderutils::PPEResultDrawable drawable(output);
  drawable.draw(base_mat, base_mat.size().width, base_mat.size().height);

  std::vector<std::vector<std::string>> table_content{{"People", "0"},
                                                      {"PPE", "0"}};
  renderutils::Table table(table_content);
  table.overlay(expected_mat, 10, 10);
  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// If we are given a detection result with only a Person (and no PPE), make sure
// the table only sets the count for person (PPE stays 0) and the bounding box
// with label are drawn in the right color with the right content/area
TEST(PPEResultDrawable, TestOnlyPersonContent) {
  cv::Mat base_mat(800, 600, CV_8UC3, kGreenColor);

  cv::Mat expected_mat = base_mat.clone();

  PersonalProtectiveEquipmentDetectionOutput output;
  DetectedPerson* person = output.add_detected_persons();
  AddPersonBox(person);

  cv::Rect expected_bounding_box(0.4 * 600, 0.5 * 800, 0.2 * 600, 0.4 * 800);
  cv::rectangle(expected_mat, expected_bounding_box, kBlueColor,
                renderutils::kLineThick2);

  DrawText(expected_mat, "0.5", expected_bounding_box);

  renderutils::PPEResultDrawable drawable(output);
  drawable.draw(base_mat, base_mat.size().width, base_mat.size().height);

  std::vector<std::vector<std::string>> table_content{{"People", "1"},
                                                      {"PPE", "0"}};
  renderutils::Table table(table_content);
  table.overlay(expected_mat, 10, 10);
  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// If we are given a detection result with a Person and PPE, make sure
// the table sets the count for both (set  to 1) and the bounding boxes
// with label are drawn in the right color with the right content/area
TEST(PPEResultDrawable, TestFullContent) {
  cv::Mat base_mat(800, 600, CV_8UC3, kGreenColor);

  cv::Mat expected_mat = base_mat.clone();

  PersonalProtectiveEquipmentDetectionOutput output;
  DetectedPerson* person = output.add_detected_persons();
  AddPersonBox(person);
  AddPPEBox(person);

  cv::Rect expected_person_bounding_box(0.4 * 600, 0.5 * 800, 0.2 * 600,
                                        0.4 * 800);
  cv::rectangle(expected_mat, expected_person_bounding_box, kBlueColor,
                renderutils::kLineThick2);

  DrawText(expected_mat, "0.5", expected_person_bounding_box);

  cv::Rect expected_ppe_bounding_box(0.5 * 600, 0.3 * 800, 0.1 * 600,
                                     0.1 * 800);
  cv::rectangle(expected_mat, expected_ppe_bounding_box, kRedColor,
                renderutils::kLineThick2);

  DrawText(expected_mat, "Helmet - 0.33", expected_ppe_bounding_box);

  renderutils::PPEResultDrawable drawable(output);
  drawable.draw(base_mat, base_mat.size().width, base_mat.size().height);

  std::vector<std::vector<std::string>> table_content{{"People", "1"},
                                                      {"PPE", "1"}};
  renderutils::Table table(table_content);
  table.overlay(expected_mat, 10, 10);
  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

}  // namespace
}  // namespace visionai
