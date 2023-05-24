#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

#
# Script that installs bazel and bazelisk in a container.
# This is meant to be used during docker image construction.

BAZEL_VERSION="4.2.1"

set -e

# Download the bazel installer.
mkdir -p /bazel
cd /bazel
if [[ ! -f "bazel-$BAZEL_VERSION-installer-linux-x86_64.sh" ]]; then
  curl -fSsL -O https://github.com/bazelbuild/bazel/releases/download/$BAZEL_VERSION/bazel-$BAZEL_VERSION-installer-linux-x86_64.sh
fi

# Run the installer and cleanup.
chmod +x /bazel/bazel-*.sh
/bazel/bazel-$BAZEL_VERSION-installer-linux-x86_64.sh
rm -f /bazel/bazel-$BAZEL_VERSION-installer-linux-x86_64.sh

# Install bazelisk
mkdir -p /usr/local/bazelisk/bin
wget -O /usr/local/bazelisk/bin/bazelisk https://github.com/bazelbuild/bazelisk/releases/latest/download/bazelisk-linux-amd64
chmod +x /usr/local/bazelisk/bin/bazelisk

# Enable bazel auto completion.
echo "source /usr/local/lib/bazel/bin/bazel-complete.bash" >> /etc/bash.bashrc
