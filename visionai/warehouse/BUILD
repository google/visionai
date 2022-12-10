# This directory is for the Warehouse libraries.

package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "warehouse_grpc_client",
    hdrs = ["warehouse_grpc_client.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//visionai/util/net/grpc:status_util",
        "@com_github_grpc_grpc//:grpc++",  # buildcleaner: keep
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "warehouse_grpc_client_test",
    srcs = ["warehouse_grpc_client_test.cc"],
    deps = [
        ":warehouse_grpc_client",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1alpha1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1alpha1:visionai_cc_proto",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "warehouse_streaming_grpc_client",
    hdrs = ["warehouse_streaming_grpc_client.h"],
    deps = [
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/net/grpc:status_util",
        "@com_github_grpc_grpc//:grpc++",  # buildcleaner: keep
        "@com_google_absl//absl/status",
    ],
)

cc_test(
    name = "warehouse_streaming_grpc_client_test",
    srcs = ["warehouse_streaming_grpc_client_test.cc"],
    deps = [
        ":warehouse_streaming_grpc_client",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util/net/grpc:client_connect",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_grpc_grpc//:grpc++",  # buildcleaner: keep
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "warehouse_ingester",
    srcs = ["warehouse_ingester.cc"],
    hdrs = ["warehouse_ingester.h"],
    deps = [
        ":warehouse_streaming_grpc_client",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util:file_helpers",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/status:status_macros",
        "//visionai/util/telemetry/metrics:labels",
        "//visionai/util/telemetry/metrics:stats",
        "@com_github_google_glog//:glog",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_grpc_grpc//:grpc++",  # buildcleaner: keep
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_test(
    name = "warehouse_ingester_test",
    srcs = ["warehouse_ingester_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":warehouse_ingester",
        "//visionai/proto/util/net/grpc:connection_options_cc_proto",
        "//visionai/util/net/grpc:client_connect",
        "//visionai/util/telemetry/metrics:labels",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_grpc",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_github_grpc_grpc//:grpc++",  # buildcleaner: keep
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
        "@com_google_googletest//:gtest_main",
    ],
)
