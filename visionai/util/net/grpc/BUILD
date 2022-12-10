package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "client_connect",
    srcs = ["client_connect.cc"],
    hdrs = ["client_connect.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":auth_token",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util:file_helpers",
        "//visionai/util:time_util",
        "//visionai/util/status:status_macros",
        "@com_github_grpc_grpc//:grpc++",  # buildcleaner: keep
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "client_connect_test",
    srcs = ["client_connect_test.cc"],
    deps = [
        ":client_connect",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "status_util",
    srcs = ["status_util.cc"],
    hdrs = ["status_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_grpc_grpc//:grpc++",  # buildcleaner: keep
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "status_util_test",
    srcs = ["status_util_test.cc"],
    deps = [
        ":status_util",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "auth_token",
    srcs = ["auth_token.cc"],
    hdrs = ["auth_token.h"],
    deps = [
        "@com_github_grpc_grpc//:gpr",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status:statusor",
    ],
)
