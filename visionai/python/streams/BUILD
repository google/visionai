# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

load("@rules_python//python:defs.bzl", "py_binary", "py_library", "py_test")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "streams",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":client",
    ],
)

py_library(
    name = "client",
    srcs = ["client.py"],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/net:channel",
    ],
)

py_test(
    name = "client_test",
    srcs = ["client_test.py"],
    deps = [
        ":client",
        "//visionai/python/gapic:visionai_v1",
        "//visionai/python/net:channel",
        "//visionai/python/testing:test",
    ],
)

py_binary(
    name = "example_streams_client",
    srcs = ["example_streams_client.py"],
    srcs_version = "PY3",
    deps = [
        ":client",
        "//visionai/python/net:channel",
        "@absl_py//absl:app",
        "@absl_py//absl/flags",
    ],
)
