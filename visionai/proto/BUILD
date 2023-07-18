load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(
    default_visibility = [
        "//cloud/ai/visionai:__subpackages__",
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],
)

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@io_bazel_rules_go//go:def.bzl", "go_library")
load("@io_bazel_rules_go//proto:def.bzl", "go_proto_library")

go_library(
    name = "cluster_selection",
    embed = [":cluster_selection_go_proto"],
    importpath = "visionai/proto/cluster_selection",
    visibility = ["//visibility:public"],
)

proto_library(
    name = "cluster_selection_proto",
    srcs = ["cluster_selection.proto"],
    visibility = ["//visibility:public"],
)

cc_proto_library(
    name = "cluster_selection_cc_proto",
    deps = [":cluster_selection_proto"],
)

go_proto_library(
    name = "cluster_selection_go_proto",
    importpath = "visionai/proto/cluster_selection_go_proto",
    proto = ":cluster_selection_proto",
    visibility = ["//visibility:public"],
)

go_library(
    name = "ingester_config",
    embed = [":ingester_config_go_proto"],
    importpath = "visionai/proto/ingester_config",
    visibility = ["//visibility:public"],
)

proto_library(
    name = "ingester_config_proto",
    srcs = ["ingester_config.proto"],
    visibility = ["//visibility:public"],
    deps = [
        ":cluster_selection_proto",
        "@com_google_protobuf//:duration_proto",
    ],
)

cc_proto_library(
    name = "ingester_config_cc_proto",
    deps = [":ingester_config_proto"],
)

go_proto_library(
    name = "ingester_config_go_proto",
    importpath = "visionai/proto/ingester_config_go_proto",
    proto = ":ingester_config_proto",
    visibility = ["//visibility:public"],
    deps = ["//visionai/proto:cluster_selection_go_proto"],
)
