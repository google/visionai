package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "status_macros",
    hdrs = ["status_macros.h"],
    deps = [
        ":status_builder",
        "//visionai/util/net/grpc:status_util",
    ],
)

cc_library(
    name = "status_builder",
    srcs = ["status_builder.cc"],
    hdrs = ["status_builder.h"],
    deps = [
        "//visionai/util:examine_stack",
        "//visionai/util:source_location",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/base:log_severity",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "status_macros_test",
    srcs = ["status_macros_test.cc"],
    deps = [
        ":status_macros",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)
