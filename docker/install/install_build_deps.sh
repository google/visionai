#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

# Install binary build dependencies required for building the SDK.

set -e

apt-get update

DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends tzdata

apt-get install -y --no-install-recommends \
    autoconf \
    automake \
    build-essential \
    ca-certificates \
    flex \
    bison \
    python3 \
    nasm \
    libjpeg-dev
