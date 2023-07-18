# This is the directory for the streams plugin framework.

load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

proto_library(
    name = "attr_value_proto",
    srcs = ["attr_value.proto"],
)

cc_proto_library(
    name = "attr_value_cc_proto",
    deps = [":attr_value_proto"],
)

proto_library(
    name = "capture_def_proto",
    srcs = ["capture_def.proto"],
    deps = [":attr_def_proto"],
)

proto_library(
    name = "attr_def_proto",
    srcs = ["attr_def.proto"],
)

cc_proto_library(
    name = "attr_def_cc_proto",
    deps = [":attr_def_proto"],
)

proto_library(
    name = "filter_def_proto",
    srcs = ["filter_def.proto"],
    deps = [":attr_def_proto"],
)

proto_library(
    name = "event_writer_def_proto",
    srcs = ["event_writer_def.proto"],
    deps = [":attr_def_proto"],
)

cc_proto_library(
    name = "event_writer_def_cc_proto",
    deps = [":event_writer_def_proto"],
)

cc_proto_library(
    name = "filter_def_cc_proto",
    deps = [":filter_def_proto"],
)

cc_proto_library(
    name = "capture_def_cc_proto",
    deps = [":capture_def_proto"],
)

cc_library(
    name = "capture_def_builder",
    srcs = [
        "capture_def_builder.cc",
    ],
    hdrs = [
        "capture_def_builder.h",
    ],
    deps = [
        ":attr_def_cc_proto",
        ":capture_def_cc_proto",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "registration",
    hdrs = [
        "registration.h",
    ],
)

cc_library(
    name = "capture_def_registry",
    srcs = [
        "capture_def_registry.cc",
    ],
    hdrs = [
        "capture_def_registry.h",
    ],
    deps = [
        ":capture_def_builder",
        ":capture_def_cc_proto",
        ":registration",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "capture_registration_test",
    srcs = ["capture_registration_test.cc"],
    deps = [
        ":capture_def_registry",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "capture_compatibility_test",
    srcs = ["capture_compatibility_test.cc"],
    deps = [
        ":attr_def_cc_proto",
        ":capture",
        ":capture_def_registry",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "attr_value_util",
    srcs = [
        "attr_value_util.cc",
    ],
    hdrs = [
        "attr_value_util.h",
    ],
    deps = [
        ":attr_value_cc_proto",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "attr_value_util_test",
    srcs = ["attr_value_util_test.cc"],
    deps = [
        ":attr_value_cc_proto",
        ":attr_value_util",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "capture",
    srcs = ["capture.cc"],
    hdrs = ["capture.h"],
    deps = [
        ":attr_value_cc_proto",
        ":attr_value_util",
        ":registration",
        "//visionai/streams/packet",
        "//visionai/util:ring_buffer",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "filter_def_builder",
    srcs = ["filter_def_builder.cc"],
    hdrs = ["filter_def_builder.h"],
    deps = [
        ":attr_def_cc_proto",
        ":filter_def_cc_proto",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "filter_def_registry",
    srcs = ["filter_def_registry.cc"],
    hdrs = ["filter_def_registry.h"],
    deps = [
        ":filter_def_builder",
        ":filter_def_cc_proto",
        ":registration",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "filter",
    srcs = ["filter.cc"],
    hdrs = ["filter.h"],
    deps = [
        ":attr_value_cc_proto",
        ":attr_value_util",
        ":registration",
        "//visionai/streams:event_manager",
        "//visionai/streams:filtered_element",
        "//visionai/streams/packet",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util:ring_buffer",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "filter_registration_test",
    srcs = ["filter_registration_test.cc"],
    deps = [
        ":filter_def_registry",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "filter_compatibility_test",
    srcs = ["filter_compatibility_test.cc"],
    deps = [
        ":attr_def_cc_proto",
        ":filter",
        ":filter_def_registry",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "event_writer_def_builder",
    srcs = ["event_writer_def_builder.cc"],
    hdrs = ["event_writer_def_builder.h"],
    deps = [
        ":attr_def_cc_proto",
        ":event_writer_def_cc_proto",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "event_writer_def_registry",
    srcs = ["event_writer_def_registry.cc"],
    hdrs = ["event_writer_def_registry.h"],
    deps = [
        ":event_writer_def_builder",
        ":event_writer_def_cc_proto",
        ":registration",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "event_writer",
    srcs = ["event_writer.cc"],
    hdrs = ["event_writer.h"],
    deps = [
        ":attr_def_cc_proto",
        ":attr_value_cc_proto",
        ":attr_value_util",
        ":event_writer_def_registry",
        ":registration",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams/packet",
        "//visionai/util:ring_buffer",
        "//visionai/util:time_util",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "event_writer_registration_test",
    srcs = ["event_writer_registration_test.cc"],
    deps = [
        ":event_writer_def_registry",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "event_writer_compatibility_test",
    srcs = ["event_writer_compatibility_test.cc"],
    deps = [
        ":attr_def_cc_proto",
        ":event_writer",
        ":event_writer_def_registry",
        "//visionai/proto:ingester_config_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)
