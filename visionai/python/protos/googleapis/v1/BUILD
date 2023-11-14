# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

load("@rules_python//python:defs.bzl", "py_library")

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "v1",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":common_pb2",
        ":common_pb2_grpc",
        ":lva_py_pb2",
        ":lva_py_pb2_grpc",
        ":streams_py_pb2",
        ":streams_py_pb2_grpc",
    ],
)

py_library(
    name = "common_pb2",
    srcs = [
        "common_pb2.py",
    ],
)

py_library(
    name = "common_pb2_grpc",
    srcs = [
        "common_pb2_grpc.py",
    ],
)

py_library(
    name = "streams_py_pb2",
    srcs = [
        "streams_resources_pb2.py",
        "streams_service_pb2.py",
    ],
    deps = [
        ":common_pb2",
    ],
)

py_library(
    name = "streams_py_pb2_grpc",
    srcs = [
        "streams_resources_pb2_grpc.py",
        "streams_service_pb2_grpc.py",
    ],
    deps = [
        ":common_pb2_grpc",
        ":streams_py_pb2",
    ],
)

py_library(
    name = "lva_py_pb2",
    srcs = [
        "lva_pb2.py",
        "lva_resources_pb2.py",
        "lva_service_pb2.py",
    ],
)

py_library(
    name = "lva_py_pb2_grpc",
    srcs = [
        "lva_pb2_grpc.py",
        "lva_resources_pb2_grpc.py",
        "lva_service_pb2_grpc.py",
    ],
    deps = [
        ":lva_py_pb2",
    ],
)
