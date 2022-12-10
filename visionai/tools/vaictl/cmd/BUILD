package(
    default_visibility = [
        "//visionai:__subpackages__",
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
