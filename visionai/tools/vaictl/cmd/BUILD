# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = [
    ],
    licenses = ["notice"],
)

load("@io_bazel_rules_go//go:def.bzl", "go_binary", "go_library")

go_library(
    name = "vaictl_lib",
    srcs = ["vaictl.go"],
    importpath = "visionai/tools/vaictl/cmd",
    visibility = ["//visibility:private"],
    deps = ["//visionai/tools/vaictl/pkg"],
)

go_binary(
    name = "vaictl",
    embed = [":vaictl_lib"],
    visibility = ["//visibility:public"],
)
