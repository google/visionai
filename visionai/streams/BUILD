# This is the directory for the Streams libraries.

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_library(
    name = "constants",
    hdrs = [
        "constants.h",
    ],
)

cc_library(
    name = "ingester",
    srcs = [
        "ingester.cc",
    ],
    hdrs = [
        "ingester.h",
    ],
    deps = [
        ":capture_module",
        ":constants",
        ":depositor_module",
        ":event_manager",
        ":filter_module",
        ":filtered_element",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams/packet",
        "//visionai/streams/util:worker",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util:ring_buffer",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "ingester_test",
    size = "small",
    srcs = ["ingester_test.cc"],
    deps = [
        ":ingester",
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/framework:event_writer_def_registry",
        "//visionai/streams/framework:filter",
        "//visionai/streams/framework:filter_def_registry",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "capture_module",
    srcs = ["capture_module.cc"],
    hdrs = ["capture_module.h"],
    deps = [
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams/framework:attr_def_cc_proto",
        "//visionai/streams/framework:attr_value_cc_proto",
        "//visionai/streams/framework:attr_value_util",
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/streams/packet",
        "//visionai/util:ring_buffer",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "filter_module",
    srcs = ["filter_module.cc"],
    hdrs = ["filter_module.h"],
    deps = [
        ":event_manager",
        ":filtered_element",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams/framework:attr_def_cc_proto",
        "//visionai/streams/framework:attr_value_cc_proto",
        "//visionai/streams/framework:attr_value_util",
        "//visionai/streams/framework:filter",
        "//visionai/streams/framework:filter_def_registry",
        "//visionai/streams/packet",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util:ring_buffer",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "depositor_module",
    srcs = ["depositor_module.cc"],
    hdrs = ["depositor_module.h"],
    deps = [
        ":constants",
        ":event_manager",
        ":event_sink",
        ":filtered_element",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams/framework:attr_def_cc_proto",
        "//visionai/streams/framework:attr_value_util",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/framework:event_writer_def_registry",
        "//visionai/streams/packet",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util:random_string",
        "//visionai/util:ring_buffer",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "event_manager",
    srcs = ["event_manager.cc"],
    hdrs = ["event_manager.h"],
    deps = [
        ":event_sink",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams/client:resource_util",
        "//visionai/util:random_string",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "event_sink",
    srcs = ["event_sink.cc"],
    hdrs = ["event_sink.h"],
    deps = [
        ":constants",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/packet",
        "//visionai/streams/util:worker",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "filtered_element",
    srcs = ["filtered_element.cc"],
    hdrs = ["filtered_element.h"],
    deps = [
        "//visionai/streams/packet",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "filtered_element_test",
    srcs = ["filtered_element_test.cc"],
    deps = [
        ":filtered_element",
        "//visionai/streams/packet",
        "@com_google_googletest//:gtest_main",
    ],
)
