# This is the directory for general utilities.
load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(
    default_visibility = [
        "//cloud/ai/visionai:__subpackages__",
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "source_location",
    hdrs = ["source_location.h"],
    deps = [
    ],
)

cc_library(
    name = "file_path",
    srcs = ["file_path.cc"],
    hdrs = ["file_path.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "file_helpers",
    srcs = ["file_helpers.cc"],
    hdrs = ["file_helpers.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":file_path",
        "@com_github_google_glog//:glog",
        "@com_github_google_re2//:re2",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "file_helpers_test",
    srcs = ["file_helpers_test.cc"],
    deps = [
        ":file_helpers",
        ":file_path",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "tuple_for_each",
    hdrs = [
        "tuple_for_each.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "tuple_apply",
    hdrs = [
        "tuple_apply.h",
    ],
    visibility = ["//visibility:public"],
)

cc_proto_library(
    name = "duration_cc_proto",
    deps = [
        "@com_google_protobuf//:duration_proto",
    ],
)

cc_proto_library(
    name = "timestamp_cc_proto",
    deps = [
        "@com_google_protobuf//:timestamp_proto",
    ],
)

cc_library(
    name = "time_util",
    srcs = ["time_util.cc"],
    hdrs = ["time_util.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":duration_cc_proto",
        ":timestamp_cc_proto",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "time_util_test",
    srcs = ["time_util_test.cc"],
    deps = [
        ":time_util",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "producer_consumer_queue",
    hdrs = [
        "producer_consumer_queue.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "producer_consumer_queue_test",
    srcs = ["producer_consumer_queue_test.cc"],
    linkstatic = 1,
    deps = [
        ":producer_consumer_queue",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "completion_signal",
    srcs = [
        "completion_signal.cc",
    ],
    hdrs = [
        "completion_signal.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "completion_signal_test",
    srcs = ["completion_signal_test.cc"],
    deps = [
        ":completion_signal",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "random_string",
    srcs = [
        "random_string.cc",
    ],
    hdrs = [
        "random_string.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/random",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "examine_stack",
    srcs = [
        "examine_stack.cc",
    ],
    hdrs = [
        "examine_stack.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/debugging:stacktrace",
        "@com_google_absl//absl/debugging:symbolize",
    ],
)

cc_test(
    name = "examine_stack_test",
    srcs = [
        "examine_stack_test.cc",
    ],
    deps = [
        ":examine_stack",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "worker",
    hdrs = ["worker.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":completion_signal",
        ":producer_consumer_queue",
    ],
)

cc_library(
    name = "work_agent",
    hdrs = [
        "work_agent.h",
    ],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "ring_buffer",
    hdrs = [
        "ring_buffer.h",
    ],
    deps = [
        "//visionai/util/gtl:circularbuffer",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "flags",
    srcs = ["flags.cc"],
    hdrs = ["flags.h"],
    deps = [
    ],
)

cc_library(
    name = "proto_util",
    srcs = ["proto_util.cc"],
    hdrs = ["proto_util.h"],
    deps = [
        ":file_helpers",
        ":line_crossing_util",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "proto_util_test",
    srcs = ["proto_util_test.cc"],
    data = ["testdata/timestamp.pbtxt"],
    deps = [
        ":proto_util",
        "@com_google_googletest//:gtest_main",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "line_crossing_util",
    srcs = ["line_crossing_util.cc"],
    hdrs = ["line_crossing_util.h"],
    deps = ["@com_google_absl//absl/strings"],
)

cc_test(
    name = "line_crossing_util_test",
    srcs = ["line_crossing_util_test.cc"],
    deps = [
        ":line_crossing_util",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "lva_runtime_util",
    srcs = ["lva_runtime_util.cc"],
    hdrs = ["lva_runtime_util.h"],
    deps = [
        "//visionai/streams/packet",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/time",
    ],
)
