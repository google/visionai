#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd
#
# Installs basic dev packages during docker image construction.

set -e

apt-get update

apt-get install -y --no-install-recommends \
    autoconf \
    automake \
    bash-completion \
    curl \
    gdb \
    tar \
    unzip \
    vim \
    wget \
    jq
