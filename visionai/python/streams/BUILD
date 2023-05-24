# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "client",
    srcs = ["client.py"],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/protos/googleapis/v1:streams_py_pb2",
        "//visionai/python/protos/googleapis/v1:streams_py_pb2_grpc",
    ],
)

py_binary(
    name = "example_streams_client",
    srcs = ["example_streams_client.py"],
    srcs_version = "PY3",
    deps = [
        ":client",
        "@absl_py//absl:app",
        "@absl_py//absl/flags",
    ],
)
