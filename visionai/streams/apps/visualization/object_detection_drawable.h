#ifndef THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_OBJECT_DETECTION_DRAWABLE_H_
#define THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_OBJECT_DETECTION_DRAWABLE_H_

#include <utility>
#include "google/cloud/visionai/v1/annotations.pb.h"
#include "opencv4/opencv2/core/mat.hpp"
#include "opencv4/opencv2/core/types.hpp"
#include "visionai/streams/apps/visualization/drawable.h"

namespace visionai {
namespace renderutils {
// ObjectDetectionDrawable is used by the Anaheim Visualization Tool.
//
// The ObjectDetectionDrawable makes use of bounding boxes and object detection
// data from ObjectDetectionPredictionResult to render bounding boxes and
// image classification annotations/confidence scores around certain objects.
class ObjectDetectionDrawable : public Drawable {
  public:
  // Constructor for instances of the ObjectDetectionDrawable given a detection
  // output object. The created ObjectDetectionDrawable will have ownership of
  // this object.
  explicit ObjectDetectionDrawable(
      google::cloud::visionai::v1::ObjectDetectionPredictionResult&
                                   object_detection_output) :
                                     object_detection_output_
                                     (std::move(object_detection_output)){}
  
  // Render in bounding boxes around identified objects given by the
  // ObjectDetectionPredictionResult. Will also render in the label of the
  // prediction result entity as well as the confidence score for the object.
  void draw(cv::Mat& image, int width, int height) override;
  
 private:
  // The object_detection_output_ contains the prediction result and the
  // information needed to draw the bounding boxes around certain objects
  google::cloud::visionai::v1::ObjectDetectionPredictionResult
      object_detection_output_;
  
  // Renders in a bounding box around a certain image based upon the
  // ObjectPredictionResult and it's identified boxes. The box will be drawn
  // in the specified color, and the label will always be drawn in white.
  void DrawBoundingBox(
      cv::Mat& image, google::cloud::visionai::v1::
      ObjectDetectionPredictionResult_IdentifiedBox_NormalizedBoundingBox
      bounding_box, cv::Scalar color, std::string label = "");
  // Calculates the starting x and y position for the object labels that are
  // included at the bottom of each bounding box
  cv::Point calcTextStartPos(cv::Mat& image, google::cloud::visionai::v1::
    ObjectDetectionPredictionResult_IdentifiedBox_NormalizedBoundingBox
    bounding_box, std::string label);
  // Calculates the ending x and y position for the object labels that are
  // included at the bottom of each bounding box
  cv::Point calcTextEndPos(cv::Mat& image, google::cloud::visionai::v1::
    ObjectDetectionPredictionResult_IdentifiedBox_NormalizedBoundingBox
    bounding_box, std::string label);
};
}
}


#endif  // THIRD_PARTY_VISIONAI_STREAMS_APPS_VISUALIZATION_GENOBJECTDETECTION_DRAWABLE_H_
