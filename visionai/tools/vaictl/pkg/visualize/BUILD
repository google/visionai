package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

load("@io_bazel_rules_go//go:def.bzl", "go_library")

go_library(
    name = "visualize",
    srcs = [
        "cli.go",
        "commands.go",
        "doc.go",
    ],
    importpath = "visionai/tools/vaictl/pkg/visualize",
    visibility = ["//visibility:public"],
    deps = [
        "//visionai/golang/pkg/streams/exec",
        "//visionai/tools/vaictl/pkg/common",
        "@com_github_spf13_cobra//:go_default_library",
        "@com_github_spf13_viper//:go_default_library",
    ],
)
