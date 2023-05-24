# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "lva",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":client",
        ":graph",
    ],
)

py_library(
    name = "client",
    srcs = ["client.py"],
    srcs_version = "PY3",
    deps = [
        ":graph",
        "//visionai/python/net:channel",
        "//visionai/python/protos/googleapis/v1:lva_py_pb2",
        "//visionai/python/protos/googleapis/v1:lva_py_pb2_grpc",
        "@com_github_grpc_grpc//src/python/grpcio/grpc:grpcio",
    ],
)

py_test(
    name = "client_test",
    srcs = ["client_test.py"],
    deps = [
        ":client",
        ":graph",
        "//visionai/python/net:channel",
        "//visionai/python/protos/googleapis/v1:lva_py_pb2",
        "//visionai/python/testing:test",
        "@com_github_grpc_grpc//src/python/grpcio/grpc:grpcio",
        "@com_google_protobuf//:protobuf_python",
    ],
)

py_library(
    name = "graph",
    srcs = ["graph.py"],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/protos/googleapis/v1:lva_py_pb2",
    ],
)

py_test(
    name = "graph_test",
    srcs = ["graph_test.py"],
    srcs_version = "PY3",
    deps = [
        ":graph",
        "//visionai/python/protos/googleapis/v1:lva_py_pb2",
        "//visionai/python/testing:test",
        "@com_google_protobuf//:protobuf_python",
    ],
)
