package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

cc_library(
    name = "mock_operations_client",
    hdrs = [
        "mock_operations_client.h",
    ],
    deps = [
        ":operations_client",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "operations_client",
    srcs = ["operations_client.cc"],
    hdrs = ["operations_client.h"],
    deps = [
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util/net:exponential_backoff",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/net/grpc:status_util",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_grpc",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_proto",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "operations_client_test",
    srcs = ["operations_client_test.cc"],
    deps = [
        ":operations_client",
        "//visionai/testing/grpc:mock_grpc",
        "//visionai/testing/status:status_matchers",
        "//visionai/util/net:exponential_backoff",
        "//visionai/util/net/grpc:client_connect",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_grpc",
        "@com_github_googleapis_googleapis//google/longrunning:longrunning_cc_proto",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)
