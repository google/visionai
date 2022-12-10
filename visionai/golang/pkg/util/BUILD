package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],  # Apache 2.0
)

load("@io_bazel_rules_go//go:def.bzl", "go_library")

go_library(
    name = "util",
    srcs = [
        "doc.go",
        "resource_util.go",
        "version.go",
    ],
    data = [
        "//debian:changelog",
    ],
    importpath = "visionai/golang/pkg/util",
    visibility = ["//visibility:public"],
)
