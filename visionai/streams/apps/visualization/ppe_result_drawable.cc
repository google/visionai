// Copyright (c) 2023 Google LLC All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "visionai/streams/apps/visualization/ppe_result_drawable.h"

#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/imgproc.hpp"
#include "absl/strings/str_cat.h"
#include "visionai/streams/apps/visualization/constants.h"
#include "visionai/streams/apps/visualization/table.h"

namespace visionai {
namespace renderutils {

const cv::Scalar kBlueColor(255, 0, 0);
const cv::Scalar kWhiteColor = cv::Scalar(255, 255, 255);
const cv::Scalar kRedColor = cv::Scalar(0, 0, 255);

void PPEResultDrawable::draw(cv::Mat& image, int width, int height) {
  int person_count = 0;
  int ppe_count = 0;

  for (const auto& detected_person : detection_output_.detected_persons()) {
    // Get the normalized bounding box for the person and draw it in blue
    const auto& detected_person_box =
        detected_person.detected_person_identified_box();
    DrawBoundingBox(image, detected_person_box.normalized_bounding_box(),
                    kBlueColor,
                    absl::StrCat(detected_person_box.confidence_score()));
    person_count++;
    for (const auto& detected_ppe_box :
         detected_person.detected_ppe_identified_boxes()) {
      // Get the normalized bounding box for the PPE and draw it in red with the
      // label including the PPE object's label
      auto& detected_ppe = detected_ppe_box.ppe_entity().ppe_label_string();
      DrawBoundingBox(image, detected_ppe_box.normalized_bounding_box(),
                      kRedColor,
                      absl::StrCat(detected_ppe, " - ",
                                   detected_ppe_box.confidence_score()));
      ppe_count++;
    }
  }

  // Draw a table with the total people and PPE counts
  std::vector<std::vector<std::string>> table_contents = {
      {"People", absl::StrCat(person_count)}, {"PPE", absl::StrCat(ppe_count)}};
  Table table(table_contents);
  table.overlay(image, 10, 10);
}

void PPEResultDrawable::DrawBoundingBox(
    cv::Mat& image,
    const google::cloud::visionai::v1::
        PersonalProtectiveEquipmentDetectionOutput_NormalizedBoundingBox& box,
    cv::Scalar color, std::string label) {
  // Normalized bounding boxes have widths and heights in percentages, so we
  // need to convert them to pixel values for this current Mat based on its
  // height and width
  int width = image.size().width;
  int height = image.size().height;
  cv::Rect rect(box.xmin() * width, box.ymin() * height, box.width() * width,
                box.height() * height);
  cv::rectangle(image, rect, color, kLineThick2);

  // Calculate where the text will be drawn. OpenCV draws starting at the bottom
  // left, and this aims to center the text under the bounding box, so we offset
  // it under the halfway point by half the width of the text
  auto text_size = cv::getTextSize(label, kTextFontFamily, kTrackTextFontscale,
                                   kStatsTextThickness, 0);
  int start_x = (rect.tl().x + rect.br().x) / 2 - text_size.width / 2;
  int start_y = rect.br().y + kLabelTextPaddingY + text_size.height;

  cv::putText(image, label, cv::Point(start_x, start_y), kTextFontFamily,
              kTrackTextFontscale, kWhiteColor, kStatsTextThickness);
}
}  // namespace renderutils
}  // namespace visionai
