#ifndef DRAWABLE_H_
#define DRAWABLE_H_
#include "opencv4/opencv2/core/mat.hpp"

// Super class to be abstracted into PPEDrawable and GeneralObjectDrawable
// Drawable class creates bounding boxes around detected images
namespace visionai {
namespace renderutils {

class Drawable {
 public:
  // Draw function that will render the image boxes based on the type of model
  // Makes use of the DrawBoundingBoxes function in the original OC tool
  // Draws bounding boxes based on identified objects from different models
  virtual void draw(cv::Mat& image, int width, int height) = 0;
  virtual ~Drawable(){}
};
}  // namespace renderutils
}  // namespace visionai
#endif  // ETHANAUYEUNG_DRAWABLE_H
