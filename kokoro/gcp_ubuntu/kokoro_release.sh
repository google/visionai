#!/bin/bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd


# Fail on any error.
set -e

# Code under repo is checked out to ${KOKORO_ARTIFACTS_DIR}/git.
# The final directory name in this path is determined by the scm name specified
# in the job configuration.
cd "${KOKORO_ARTIFACTS_DIR}/git/workspace-v2"

docker run --rm  \
  -w /workdir \
  -v "$PWD":/workdir \
  "gcr.io/visionai-dev-project/visionai-dev-nosdk:latest" \
  ./release.sh \

gsutil cp -r artifacts/ gs://visionai-private-artifacts/$(git rev-parse HEAD)
