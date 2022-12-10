package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

load("@io_bazel_rules_go//go:def.bzl", "go_library")

go_library(
    name = "pkg",
    srcs = [
        "cli.go",
        "doc.go",
    ],
    importpath = "visionai/tools/vaictl/pkg",
    visibility = ["//visibility:public"],
    deps = [
        "//visionai/golang/pkg/util",
        "//visionai/tools/vaictl/pkg/common",
        "//visionai/tools/vaictl/pkg/crud",
        "//visionai/tools/vaictl/pkg/receive",
        "//visionai/tools/vaictl/pkg/send",
        "//visionai/tools/vaictl/pkg/visualize",
        "@com_github_spf13_cobra//:go_default_library",
        "@com_github_spf13_viper//:go_default_library",
    ],
)
