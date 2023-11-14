# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

load("@rules_python//python:defs.bzl", "py_binary")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_binary(
    name = "echo",
    srcs = [
        "echo.py",
    ],
    srcs_version = "PY3",
    deps = [
        "@absl_py//absl:app",
    ],
)

py_binary(
    name = "blur_gcs_video",
    srcs = ["blur_gcs_video.py"],
    deps = [
        "//visionai/python/lva:client",
        "//visionai/python/net:channel",
        "//visionai/python/prediction:offline",
        "@absl_py//absl:app",
        "@absl_py//absl/flags",
    ],
)
