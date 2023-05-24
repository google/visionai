#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

#
# Builds and installs OpenCV from source during docker image construction.
#
# TODO(dschao): Remove this if MediaPipe will use the default OpenCV?
# It is currently kept because we are bootstrapping the development
# environment.

set -e

# Install build essential for opencv.
apt-get -y install build-essential cmake git libgtk2.0-dev pkg-config \
  libavcodec-dev libavformat-dev libswscale-dev python-dev python-numpy \
  libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-22-dev

OPENCV_VERSION=4.7.0
OPENCV_BUILD_DIR=/tmp/build_opencv

# Download OpenCV source.
rm -rf $OPENCV_BUILD_DIR
mkdir $OPENCV_BUILD_DIR
cd $OPENCV_BUILD_DIR
git clone https://github.com/opencv/opencv_contrib.git
pushd opencv_contrib
git checkout tags/$OPENCV_VERSION -b v$OPENCV_VERSION
popd
git clone https://github.com/opencv/opencv.git
pushd opencv
git checkout tags/$OPENCV_VERSION -b v$OPENCV_VERSION
popd

# Directory to install opencv
mkdir -p /usr/local/opencv

# Build and install using CMake/Make.
mkdir opencv/release
pushd opencv/release
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_INSTALL_PREFIX=/usr/local/opencv \
      -DBUILD_TESTS=OFF -DBUILD_PERF_TESTS=OFF -DBUILD_opencv_ts=OFF \
      -DOPENCV_EXTRA_MODULES_PATH=$OPENCV_BUILD_DIR/opencv_contrib/modules \
      -DBUILD_opencv_aruco=OFF -DBUILD_opencv_bgsegm=OFF -DBUILD_opencv_bioinspired=OFF \
      -DBUILD_opencv_ccalib=OFF -DBUILD_opencv_datasets=OFF -DBUILD_opencv_dnn=OFF \
      -DBUILD_opencv_dnn_objdetect=OFF -DBUILD_opencv_dpm=OFF -DBUILD_opencv_face=OFF \
      -DBUILD_opencv_fuzzy=OFF -DBUILD_opencv_hfs=OFF -DBUILD_opencv_img_hash=OFF \
      -DBUILD_opencv_js=OFF -DBUILD_opencv_line_descriptor=OFF -DBUILD_opencv_phase_unwrapping=OFF \
      -DBUILD_opencv_plot=OFF -DBUILD_opencv_quality=OFF -DBUILD_opencv_reg=OFF \
      -DBUILD_opencv_rgbd=OFF -DBUILD_opencv_saliency=OFF -DBUILD_opencv_shape=OFF \
      -DBUILD_opencv_structured_light=OFF -DBUILD_opencv_surface_matching=OFF \
      -DBUILD_opencv_world=OFF -DBUILD_opencv_xobjdetect=OFF -DBUILD_opencv_xphoto=OFF
make -j
make install
rm -rf $OPENCV_BUILD_DIR
ln -s /usr/local/opencv/include/opencv4/opencv2 /usr/local/include/opencv2

# Add opencv directory to default library path
echo "/usr/local/opencv/lib" >> /etc/ld.so.conf.d/opencv.conf
