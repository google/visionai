# This directory is for event writer plugins.

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_library(
    name = "all_event_writers",
    deps = [
        ":encoded_stream_log_event_writer",
        ":local_video_event_writer",
        ":log_event_writer",
        ":streams_event_writer",
        ":streams_jpeg_event_writer",
    ],
    alwayslink = 1,
)

cc_library(
    name = "streams_event_writer",
    srcs = ["streams_event_writer.cc"],
    hdrs = ["streams_event_writer.h"],
    deps = [
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/streams/client:control",
        "//visionai/streams/client:descriptors",
        "//visionai/streams/client:packet_sender",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/framework:event_writer_def_registry",
        "//visionai/util:random_string",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "streams_jpeg_event_writer",
    srcs = ["streams_jpeg_event_writer.cc"],
    hdrs = ["streams_jpeg_event_writer.h"],
    deps = [
        "//visionai/algorithms/media:gstreamer_async_jpeg_encoder",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/streams/client:control",
        "//visionai/streams/client:descriptors",
        "//visionai/streams/client:packet_sender",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/framework:event_writer_def_registry",
        "//visionai/util:file_helpers",
        "//visionai/util:file_path",
        "//visionai/util:random_string",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
    alwayslink = 1,
)

cc_library(
    name = "local_video_event_writer",
    srcs = ["local_video_event_writer.cc"],
    hdrs = ["local_video_event_writer.h"],
    deps = [
        "//visionai/algorithms/media:gstreamer_video_writer",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/framework:event_writer_def_registry",
        "//visionai/streams/packet",
        "//visionai/util:file_path",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
    alwayslink = 1,
)

cc_library(
    name = "log_event_writer",
    srcs = ["log_event_writer.cc"],
    hdrs = ["log_event_writer.h"],
    deps = [
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/streams/client:control",
        "//visionai/streams/client:descriptors",
        "//visionai/streams/client:packet_sender",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/framework:event_writer_def_registry",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
    alwayslink = 1,
)

cc_library(
    name = "encoded_stream_log_event_writer",
    srcs = ["encoded_stream_log_event_writer.cc"],
    deps = [
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/streams/framework:event_writer",
        "//visionai/streams/framework:event_writer_def_registry",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
    alwayslink = 1,
)
