#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

#
# Script to configure nvidia libraires during docker image construction.

set -e

# Needed for nvidia/cuda:10.0-cudnn7-devel-ubuntu18.04.
# Manually place CUDNN next to usual cuda libraries.
cp -P /usr/include/cudnn.h /usr/local/cuda/include
cp -P /usr/lib/x86_64-linux-gnu/libcudnn* /usr/local/cuda/lib64

# Needed for nvidia/cuda:10.0-cudnn7-devel-ubuntu18.04.
# Manually place NCCL next to usual cuda libraries.
mkdir /usr/local/cuda/lib
ln -s /usr/lib/x86_64-linux-gnu/libnccl.so.2 /usr/local/cuda/lib/libnccl.so.2
ln -s /usr/include/nccl.h /usr/local/cuda/include/nccl.h
ldconfig

# Needed for tensorrt.
wget https://developer.download.nvidia.com/compute/machine-learning/repos/ubuntu1804/x86_64/libnvinfer6_6.0.1-1+cuda10.0_amd64.deb
apt install ./libnvinfer6_6.0.1-1+cuda10.0_amd64.deb
apt-get update

apt-get install -y  --no-install-recommends libnvinfer6=6.0.1-1+cuda10.0 \
  libnvinfer-dev=6.0.1-1+cuda10.0 libnvinfer-plugin6=6.0.1-1+cuda10.0 \
  libnvinfer-plugin-dev=6.0.1-1+cuda10.0
