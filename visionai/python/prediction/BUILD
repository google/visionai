# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "prediction",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":offline",
    ],
)

py_library(
    name = "offline",
    srcs = [
        "offline.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/lva:client",
        "//visionai/python/net:channel",
        "//visionai/python/recipes:blur_gcs_video",
    ],
)

py_test(
    name = "offline_test",
    srcs = ["offline_test.py"],
    srcs_version = "PY3",
    deps = [
        ":offline",
        "//visionai/python/net:channel",
        "//visionai/python/testing:test",
    ],
)
