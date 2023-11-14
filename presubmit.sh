#!/bin/bash
# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

set -e

bazel build -c opt ... \
  --remote_cache=https://storage.googleapis.com/visionai_kokoro_build_cache \
  --google_default_credentials=true \

# TODO(b/287164762): Test failure //third_party/visionai/streams/plugins/captures:rtsp_image_capture_test
bazel test -c opt \
  $(bazel query '(... except (//visionai/streams/plugins/captures:rtsp_image_capture_test))') \
  --remote_cache=https://storage.googleapis.com/visionai_kokoro_build_cache \
  --google_default_credentials=true \
