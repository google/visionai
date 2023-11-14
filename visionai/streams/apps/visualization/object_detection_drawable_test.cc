// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/apps/visualization/object_detection_drawable.h"

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "gtest/gtest.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "visionai/streams/apps/visualization/constants.h"
#include "visionai/streams/apps/visualization/test_utils.h"


// Testing file for the ObjectDetectionDrawable
namespace visionai{
namespace renderutils{

const cv::Scalar kBlackColor = cv::Scalar(0, 0, 0);
const cv::Scalar kBlueColor(255, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kRedColor = cv::Scalar(0, 0, 255);
const cv::Scalar kGreenColor(0, 255, 0);

// Class declarations for code for code consistency and readability
using google::cloud::visionai::v1::ObjectDetectionPredictionResult;
using DetectedObject =
    google::cloud::visionai::v1::ObjectDetectionPredictionResult_IdentifiedBox;
using NormalizedBoundingBox = google::cloud::visionai::v1::
    OccupancyCountingPredictionResult_IdentifiedBox_NormalizedBoundingBox;

// Adds an Object bounding box for the detected object. Used to generate test
// objects for tests and to correct the accuracy of the Object bounding boxes
// and annotations that the ObjectDetectionDrawable renders in
void AddObjectBox(DetectedObject* object) {
  auto object_box = object->mutable_normalized_bounding_box();
  
  object_box->set_xmin(0.5);
  object_box->set_width(0.4);
  object_box->set_ymin(0.6);
  object_box->set_height(0.5);
  
  object->set_confidence_score(0.5);
  object->mutable_entity()->set_label_string("Table");
}

void AddSecondObjectBox(DetectedObject* object) {
  auto object_box = object->mutable_normalized_bounding_box();
  
  object_box->set_xmin(0.5);
  object_box->set_width(0.4);
  object_box->set_ymin(0.6);
  object_box->set_height(0.5);
  
  object->set_confidence_score(0.35);
  object->mutable_entity()->set_label_string("Wood");
}

// Helper method to draw text in the same place it should appear in the actual
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

// Test to see that if there is no detectable objects, no bounding boxes will
// rendered onto the screen.
TEST(ObjectDetectionDrawable, TestNoInfo) {
  cv::Mat base_mat(mat_rows, mat_cols, CV_8UC3, kGreenColor);
  cv:: Mat expected_mat = base_mat.clone();
  
  ObjectDetectionPredictionResult object_detection_result;
  ObjectDetectionDrawable object_detection_output(object_detection_result);
  
  object_detection_output.draw(
      base_mat, base_mat.size().width, base_mat.size().height);
  
  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
}

// Make sure that if you are given a certain object the draw function in
// object_detection_drawable.h will correctly render in a bounding box around
// that object and classify it correctly

TEST(ObjectDetectionDrawable, TestOneObject) {
  cv::Mat base_mat(mat_rows, mat_cols, CV_8UC3, kBlueColor);
  
  cv::Mat expected_mat = base_mat.clone();
  
  ObjectDetectionPredictionResult object_detection_result;
  DetectedObject* object = object_detection_result.add_identified_boxes();
  AddObjectBox(object);
  
  cv::Rect expected_bounding_box(0.5 * 600, 0.6 * 800, 0.4 * 600, 0.5 * 800);
  cv::rectangle(expected_mat, expected_bounding_box, kBlueColor, kLineThick2);
  
  DrawText(expected_mat, "Table - 0.5", expected_bounding_box);
  
  ObjectDetectionDrawable object_detection_output(object_detection_result);
  object_detection_output.draw(base_mat, base_mat.size().width,
                               base_mat.size().height);
  
  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
  
}

// Make sure that if there are multiple objects within a single frame the draw
// function in object_detection_drawable.h will correctly render in a bounding
// box around that object and classify it correctly

TEST(ObjectDetectionDrawable, TestMultpleObjects) {
  cv::Mat base_mat(mat_rows, mat_cols, CV_8UC3, kBlueColor);
  
  cv::Mat expected_mat = base_mat.clone();
  int num_objects = 3;
  
  ObjectDetectionPredictionResult object_detection_result;
  for (int i = 0; i < num_objects; i++) {
    DetectedObject* object = object_detection_result.add_identified_boxes();
    AddObjectBox(object);
  }
  
  cv::Rect expected_bounding_box(0.5 * 600, 0.6 * 800, 0.4 * 600, 0.5 * 800);
  cv::rectangle(expected_mat, expected_bounding_box, kBlueColor, kLineThick2);
  
  for (int i = 0; i < num_objects; i++) {
    DrawText(expected_mat, "Table - 0.5", expected_bounding_box);
  } 
  
  ObjectDetectionDrawable object_detection_output(object_detection_result);
  object_detection_output.draw(base_mat, base_mat.size().width,
                               base_mat.size().height);
  
  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
  
}

// Create two bounding boxes for the same object with different labels and
// confidence scores and test to see that only the bounding box with the
// higher confidence score gets rendered onto the object.
TEST(ObjectDetectionDrawable, TestOverlayingText) {
  cv::Mat base_mat(mat_rows, mat_cols, CV_8UC3, kBlueColor);
  
  cv::Mat expected_mat = base_mat.clone();
  
  ObjectDetectionPredictionResult object_detection_result;
  DetectedObject* object = object_detection_result.add_identified_boxes();
  AddObjectBox(object);
  DetectedObject* objectTwo = object_detection_result.add_identified_boxes();
  AddSecondObjectBox(objectTwo);
  
  ObjectDetectionDrawable object_detection_output(object_detection_result);
  object_detection_output.draw(base_mat, base_mat.size().width,
                               base_mat.size().height);
  
  cv::Rect expected_bounding_box_one(
      0.5 * 600, 0.6 * 800, 0.4 * 600, 0.5 * 800);
  cv::rectangle(expected_mat, expected_bounding_box_one,
                kBlueColor, kLineThick2);

  
  DrawText(expected_mat, "Table - 0.5", expected_bounding_box_one);
  
  EXPECT_TRUE(testutils::CheckTwoImagesEqual(base_mat, expected_mat));
  
  
}

}  // namespace renderutils
}  // namespace visionai
