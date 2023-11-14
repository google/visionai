/*
 * Copyright (c) 2023 Google LLC All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_PPE_RESULT_DRAWABLE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_PPE_RESULT_DRAWABLE_H_

#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "visionai/streams/apps/visualization/drawable.h"
namespace visionai {
namespace renderutils {
// PPEResultDrawable is used by the Anaheim Visualization Tool.
//
// PPEResultDrawable utilizes the bounding box and person/PPE detection data
// from PersonalProtectiveEquipmentDetectionOutput. The information from that
// obejct class is used to get information as to where on the video stream the
// bounding boxes are located for different object detections. It uses this
// information with the confidence score and string label (of PPE entities only)
// to draw diagnostic information onto the video stream within the Anaheim
// Visualization Tool. The intended use-case for this class is as follows:
//
// 1. To draw diagnostic information onto a video stream with a corresponding
// Packet Stream whose payloads are PersonalProtectiveEquipmentDetectionOutput
// objects.
class PPEResultDrawable : public Drawable {
 public:
  // Constructs a new instance of PPEResultDrawable given a source output
  // object. This operation will move the detection output and claim ownership of
  // that object
  explicit PPEResultDrawable(
      google::cloud::visionai::v1::PersonalProtectiveEquipmentDetectionOutput&
          detection_output)
      : detection_output_(std::move(detection_output)) {}

  // Overlay bounding boxes, labels, and a table for detected people and PPE.
  // People's bounding boxes will have labels with just the confidence score
  // while PPE boxes will have a label containing the object name as well. The
  // table shows total people and PPE count.
  void draw(cv::Mat& image, int width, int height) override;

 private:
  // The PersonalProtectiveEquipmentDetectionOutput object that contains all of
  // the information as to where on the video stream the bounding boxes should
  // be drawn
  google::cloud::visionai::v1::PersonalProtectiveEquipmentDetectionOutput
      detection_output_;

  // Helper method to draw bounding boxes with the given
  // PersonalProtectiveEquipmentDetectionOutput_NormalizedBoundingBox as this is
  // shared between PPE and Person objects. Will draw the box in the color
  // specified while label is drawn in white.
  void DrawBoundingBox(
      cv::Mat& image,
      const google::cloud::visionai::v1::
          PersonalProtectiveEquipmentDetectionOutput_NormalizedBoundingBox& box,
      cv::Scalar color, std::string label = "");
};
}  // namespace renderutils
}  // namespace visionai
#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_PPE_RESULT_DRAWABLE_H_
