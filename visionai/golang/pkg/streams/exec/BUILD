# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

load("@io_bazel_rules_go//go:def.bzl", "go_library")

go_library(
    name = "exec",
    srcs = [
        "doc.go",
        "helpers.go",
        "ingest.go",
        "options.go",
        "paths.go",
        "receive_cat.go",
        "resource_management.go",
        "visualization_app.go",
    ],
    data = [
        "//visionai/streams/apps:ingester_app",
        "//visionai/streams/apps:receive_cat_app",
        "//visionai/streams/apps:resource_manager_app",
        "//visionai/streams/apps/visualization:oc_visualization_app",
    ],
    importpath = "visionai/golang/pkg/streams/exec",
    visibility = ["//visibility:public"],
    deps = [
        "//visionai/proto:ingester_config_go_proto",
        "@com_github_golang_protobuf//proto:go_default_library",
    ],
)
