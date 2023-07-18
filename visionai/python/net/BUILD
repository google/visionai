# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "net",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":channel",
    ],
)

py_library(
    name = "channel",
    srcs = ["channel.py"],
)

py_test(
    name = "channel_test",
    srcs = ["channel_test.py"],
    srcs_version = "PY3",
    deps = [
        ":channel",
        "//visionai/python/testing:test",
    ],
)
