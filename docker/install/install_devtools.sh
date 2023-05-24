#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

# Install tools useful during development.

set -e

apt-get update

# You should insert build dependencies in install_build_deps.sh.
# These are strictly productivity tools during development.
apt-get install -y --no-install-recommends \
    bash-completion \
    clang-format \
    cmake \
    curl \
    devscripts \
    fakeroot \
    gdb \
    git \
    graphviz \
    libboost-all-dev \
    tar \
    unzip \
    vim \
    wget \
    jq \
    patchelf
