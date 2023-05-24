# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

load("@io_bazel_rules_go//go:def.bzl", "go_library")

go_library(
    name = "crud",
    srcs = [
        "cli.go",
        "cluster.go",
        "doc.go",
        "helpers.go",
        "stream.go",
        "util.go",
    ],
    importpath = "visionai/tools/vaictl/pkg/crud",
    visibility = ["//visibility:public"],
    deps = [
        "//visionai/golang/pkg/streams/exec",
        "//visionai/tools/vaictl/pkg/common",
        "@com_github_spf13_cobra//:go_default_library",
        "@com_github_spf13_viper//:go_default_library",
    ],
)
