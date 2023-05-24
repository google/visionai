# This assumes that OpenCV is installed on your system.
#
# Please see docker/install/install_opencv_from_source.sh to see
# which version and components of OpenCV is expected to be built and
# installed onto your system.
# File simplified from: https://github.com/google/mediapipe/blob/v0.8.11/third_party/opencv_linux.BUILD
# Follow the instructions to install opencv there (pick ubuntu:focal for opencv 3.4).
cc_library(
  name = "opencv",
  srcs = glob(
    [
        "lib/libopencv_core.so",
        "lib/libopencv_highgui.so",
        "lib/libopencv_imgcodecs.so",
        "lib/libopencv_imgproc.so",
        "lib/libopencv_video.so",
        "lib/libopencv_videoio.so",
    ],
  ),
  hdrs = glob(["include/opencv4/opencv2/**/*.h*"]),
  includes = ["include"],
  visibility = ["//visibility:public"],
)
