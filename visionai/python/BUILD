# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

load("@rules_python//python:defs.bzl", "py_library")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "python",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/lva",
        "//visionai/python/net",
        "//visionai/python/ops",
        "//visionai/python/prediction",
        "//visionai/python/recipes",
        "//visionai/python/streams",
        "//visionai/python/warehouse",
    ],
)
