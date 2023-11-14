# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

load("@rules_python//python:defs.bzl", "py_library")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "recipes",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":blur_gcs_video",
    ],
)

py_library(
    name = "blur_gcs_video",
    srcs = ["blur_gcs_video.py"],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/lva:graph",
        "//visionai/python/ops:gen_ops",
    ],
)
