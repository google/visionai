# This directory is for filter plugins.

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_library(
    name = "all_filters",
    deps = [
        ":encoded_motion_filter",
        ":noop_filter",
        ":simple_segment_filter",
    ],
    alwayslink = 1,
)

cc_library(
    name = "noop_filter",
    srcs = ["noop_filter.cc"],
    deps = [
        "//visionai/streams/framework:filter",
        "//visionai/streams/framework:filter_def_registry",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "simple_segment_filter",
    srcs = ["simple_segment_filter.cc"],
    deps = [
        "//visionai/streams/framework:filter",
        "//visionai/streams/framework:filter_def_registry",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "encoded_motion_filter",
    srcs = ["encoded_motion_filter.cc"],
    hdrs = ["encoded_motion_filter.h"],
    deps = [
        "//visionai/algorithms/detection/motion_detection:motion_vector_based_motion_detector",
        "//visionai/algorithms/detection/motion_detection:motion_vector_based_motion_detector_config_cc_proto",
        "//visionai/algorithms/media:gstreamer_async_motion_decoder",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/streams/framework:filter",
        "//visionai/streams/framework:filter_def_registry",
        "//visionai/streams/packet",
        "//visionai/streams/util:h264_frame_buffer",
        "//visionai/types:gstreamer_buffer",
        "//visionai/types:motion_vector",
        "//visionai/types:raw_image",
        "//visionai/util:file_helpers",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)
