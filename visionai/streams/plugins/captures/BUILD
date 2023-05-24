# This directory is for capture plugins.
package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_library(
    name = "all_captures",
    deps = [
        ":bytes_sender_capture",
        ":file_source_capture",
        ":file_source_image_capture",
        ":getline_capture",
        ":int_sequence_capture",
        ":rtsp_capture",
        ":rtsp_image_capture",
        ":streaming_service_capture",
    ],
    alwayslink = 1,
)

cc_library(
    name = "int_sequence_capture",
    srcs = ["int_sequence_capture.cc"],
    deps = [
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "getline_capture",
    srcs = ["getline_capture.cc"],
    deps = [
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/streams/packet",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "bytes_sender_capture",
    srcs = ["bytes_sender_capture.cc"],
    hdrs = ["bytes_sender_capture.h"],
    deps = [
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/streams/packet",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_test(
    name = "bytes_sender_capture_test",
    srcs = ["bytes_sender_capture_test.cc"],
    deps = [
        ":bytes_sender_capture",
        "//visionai/algorithms/media/util:gstreamer_registry",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams:capture_module",
        "//visionai/streams/packet",
        "//visionai/streams/util:worker",
        "//visionai/util:ring_buffer",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "rtsp_image_capture",
    srcs = ["rtsp_image_capture.cc"],
    hdrs = ["rtsp_image_capture.h"],
    deps = [
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/util:producer_consumer_queue",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_protobuf//:protobuf",
    ],
    alwayslink = 1,
)

cc_test(
    name = "rtsp_image_capture_test",
    srcs = [
        "rtsp_image_capture_test.cc",
    ],
    local = True,
    deps = [
        ":rtsp_image_capture",
        "//visionai/algorithms/media/util:gstreamer_registry",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams:capture_module",
        "//visionai/streams/packet",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:ring_buffer",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "rtsp_capture",
    srcs = ["rtsp_capture.cc"],
    hdrs = ["rtsp_capture.h"],
    deps = [
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/algorithms/media/util",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "streaming_service_capture",
    srcs = ["streaming_service_capture.cc"],
    deps = [
        "//visionai/streams/client:constants",
        "//visionai/streams/client:descriptors",
        "//visionai/streams/client:event_update",
        "//visionai/streams/client:event_update_receiver",
        "//visionai/streams/client:packet_receiver",
        "//visionai/streams/client:resource_util",
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/util/status:status_macros",
        "//visionai/util/telemetry/metrics:stats",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "file_source_capture",
    srcs = ["file_source_capture.cc"],
    hdrs = [
        "file_source_capture.h",
    ],
    deps = [
        "//visionai/algorithms/media/util:codec_validator",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/streams:constants",
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/util:file_helpers",
        "//visionai/util/gstreamer:pipeline_string",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_protobuf//:protobuf",
    ],
    alwayslink = 1,
)

cc_test(
    name = "file_source_capture_test",
    srcs = ["file_source_capture_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":file_source_capture",
        "//visionai/algorithms/media/util:gstreamer_registry",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams:capture_module",
        "//visionai/streams/packet",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:file_path",
        "//visionai/util:ring_buffer",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "file_source_image_capture",
    srcs = ["file_source_image_capture.cc"],
    hdrs = ["file_source_image_capture.h"],
    deps = [
        "//visionai/algorithms/media/util:codec_validator",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/streams:constants",
        "//visionai/streams/framework:capture",
        "//visionai/streams/framework:capture_def_registry",
        "//visionai/util:file_helpers",
        "//visionai/util:producer_consumer_queue",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_protobuf//:protobuf",
    ],
    alwayslink = 1,
)

cc_test(
    name = "file_source_image_capture_test",
    srcs = [
        "file_source_image_capture_test.cc",
    ],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":file_source_image_capture",
        "//visionai/algorithms/media/util:gstreamer_registry",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams:capture_module",
        "//visionai/streams/packet",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:file_path",
        "//visionai/util:ring_buffer",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)
