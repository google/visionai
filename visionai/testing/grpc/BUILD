load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_library(
    name = "mock_grpc",
    hdrs = ["mock_grpc.h"],
    deps = [
        "//visionai/util/tuple:for_each",
        "@com_github_grpc_grpc//:grpc++_public_hdrs",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_googletest//:gtest",
    ],
)

proto_library(
    name = "echo",
    srcs = ["echo.proto"],
)

cc_proto_library(
    name = "echo_cc_proto",
    deps = [":echo"],
)

cc_grpc_library(
    name = "echo_cc_grpc_proto",
    srcs = [":echo"],
    grpc_only = True,
    deps = [":echo_cc_proto"],
)

cc_library(
    name = "mock_echo_service",
    hdrs = [
        "mock_echo_service.h",
    ],
    deps = [
        ":echo_cc_grpc_proto",
        ":echo_cc_proto",
        ":mock_grpc",
        "@com_google_googletest//:gtest_main",
    ],
)
