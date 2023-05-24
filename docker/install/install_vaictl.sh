#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd


set -e

# Install the pre-built vaictl
dpkg -i /artifacts/visionai-sdk_0.0-1_amd64.deb
apt-get install -f

# Enable vaictl bash completion.
echo "source <(vaictl completion bash)" >> /etc/bash.bashrc
