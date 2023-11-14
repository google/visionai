// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/apps/visualization/object_detection_drawable.h"

#include <algorithm>
#include <vector>

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "absl/strings/str_cat.h"
#include "visionai/streams/apps/visualization/constants.h"


namespace visionai {
namespace renderutils {

const cv::Scalar kBlueColor(255, 0, 0);
const cv::Scalar kGreenColor(0, 255, 0);
const cv::Scalar kWhiteColor(255, 255, 255);
const double objectDetectionFontScale = 0.28;

// The draw method renders in bounding boxes around detected objects from the
// Object Detection model. These boxes include annotations recognizing the type
// of object and it's confidence score.This method also makes use of bounding
// logic to ensure that there is no overlap between boxes and text that may make
// the annotations illegible.
void ObjectDetectionDrawable :: draw(cv::Mat& image, int width, int height) {
  std::vector<std::vector<cv::Point>> drawn_objects;
  ::google::protobuf::RepeatedPtrField<::google::cloud::visionai::v1::
                             ObjectDetectionPredictionResult_IdentifiedBox>
  sorted_identified_boxes = object_detection_output_.identified_boxes();
  // Sort the identified boxes by confidence score so only those of the highest
  // confidence scores get drawn to avoid overlap in boxes and labels.
  std::sort(sorted_identified_boxes.begin(),
            sorted_identified_boxes.end(),
            [](auto& a, auto& b) {
              return a.confidence_score() > b.confidence_score();
            });
    for (const auto& detected_object_box:
            sorted_identified_boxes) {
      bool box_exists = false;
      std::string detected_object = "";
      // Ensures that if the label is too large it will be split into multiple
      // lines.
      if (detected_object_box.entity().label_string().size() > 15) {
        detected_object =
            absl::StrCat(detected_object_box.entity().label_string());
      } else {
        detected_object =
            absl::StrCat(detected_object_box.entity().label_string());
      }
      cv::Point text_start =
              calcTextStartPos(image,
                               detected_object_box.normalized_bounding_box(),
                               detected_object);
          cv::Point text_end =
              calcTextEndPos(image,
                             detected_object_box.normalized_bounding_box(),
                             detected_object);
      if (!drawn_objects.empty()) {
        for (int i = 0; i < drawn_objects.size(); ++i) {
          // Logic that makes sure that bounding boxes for newly identified
          // objects do not overlap any other existing bounding boxes on the
          // screen.
          bool overlap_X = (text_start.x <= drawn_objects[i][0].x &&
                            text_end.x >= drawn_objects[i][0].x) ||
                           (text_start.x >= drawn_objects[i][0].x &&
                            text_start.x <= drawn_objects[i][1].x);

          bool overlap_Y = (text_start.y <= drawn_objects[i][0].y &&
                            text_end.y >= drawn_objects[i][0].y) ||
                           (text_start.y >= drawn_objects[i][0].y &&
                            text_start.y <= drawn_objects[i][1].y);
          if (overlap_X && overlap_Y) {
            box_exists = true;
            break;
          }
        }
        if (!box_exists) {
          DrawBoundingBox(
                  image, detected_object_box.normalized_bounding_box(),
                  kBlueColor,
                  absl::StrCat(detected_object, "-",
                              detected_object_box.confidence_score()));
              drawn_objects.push_back(
                  {text_start, text_end});
        }
      } else {
        DrawBoundingBox(image, detected_object_box.normalized_bounding_box(),
                        kBlueColor,
                        absl::StrCat(detected_object, "-",
                                     detected_object_box.confidence_score()));
        drawn_objects.push_back({text_start, text_end});
      }
}
}

void ObjectDetectionDrawable :: DrawBoundingBox(
    cv::Mat& image, google::cloud::visionai::v1::
    ObjectDetectionPredictionResult_IdentifiedBox_NormalizedBoundingBox
    bounding_box, cv::Scalar color, std::string label) {
    // Convert the normalized bounding box into pixel values for the Mat based
    // on width an height.
    int width = image.size().width;
    int height = image.size().height;
    cv::Rect rect(bounding_box.xmin() * width, bounding_box.ymin() * height,
                  bounding_box.width() * width, bounding_box.height() * height);
    cv::rectangle(image, rect, color, kLineThick2);
    // Modify the text placement to place the text right under the bounding box
    cv::Point startingPoint = calcTextStartPos(image, bounding_box, label);
    cv::putText(image, label, startingPoint, kTextFontFamily,
               objectDetectionFontScale, kWhiteColor, kStatsTextThickness);
  }

cv::Point ObjectDetectionDrawable :: calcTextStartPos(cv::Mat& image,
                                               google::cloud::visionai::v1::
    ObjectDetectionPredictionResult_IdentifiedBox_NormalizedBoundingBox
    bounding_box, std::string label) {
  int width = image.size().width;
  int height = image.size().height;
  cv::Rect rect(bounding_box.xmin() * width, bounding_box.ymin() * height,
                  bounding_box.width() * width, bounding_box.height() * height);
  auto text = cv::getTextSize(label, kTextFontFamily, objectDetectionFontScale,
                                kStatsTextThickness, 0);
  int start_x = (rect.tl().x + rect.br().x) / 2 - text.width / 2;
  int start_y = rect.br().y + kLabelTextPaddingY + text.height;
  
  cv::Point start = cv::Point(start_x, start_y);
  
  return start;
}

cv::Point ObjectDetectionDrawable :: calcTextEndPos(cv::Mat& image,
                                             google::cloud::visionai::v1::
    ObjectDetectionPredictionResult_IdentifiedBox_NormalizedBoundingBox
    bounding_box, std::string label) {
  int width = image.size().width;
  int height = image.size().height;
  cv::Rect rect(bounding_box.xmin() * width, bounding_box.ymin() * height,
                  bounding_box.width() * width, bounding_box.height() * height);
  auto text = cv::getTextSize(label, kTextFontFamily, objectDetectionFontScale,
                                kStatsTextThickness, 0);
  int start_x = (rect.tl().x + rect.br().x) / 2 - text.width / 2;
  int start_y = rect.br().y + kLabelTextPaddingY + text.height;
  
  int end_x = start_x + text.width;
  int end_y = start_y + text.height;
  
  cv::Point end = cv::Point(end_x, end_y);
  
  return end;
}

}  // namespace renderutils
}  // namespace visionai
