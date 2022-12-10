# This assumes that OpenCV is installed on your system.
#
# Please see docker/install/install_opencv_from_source.sh to see
# which version and components of OpenCV is expected to be built and
# installed onto your system.
# File simplified from: https://github.com/google/mediapipe/blob/v0.8.11/third_party/opencv_linux.BUILD
# Follow the instructions to install opencv there (pick ubuntu:focal for opencv 3.4).
cc_library(
  name = "opencv",
  linkopts = [
    "-l:libopencv_core.so",
    "-l:libopencv_calib3d.so",
    "-l:libopencv_features2d.so",
    "-l:libopencv_highgui.so",
    "-l:libopencv_imgcodecs.so",
    "-l:libopencv_imgproc.so",
    "-l:libopencv_video.so",
    "-l:libopencv_videoio.so",
  ],
  visibility = ["//visibility:public"],
)
