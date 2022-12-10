# This is the directory for the customer facing C++ public interfaces.

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_library(
    name = "streams",
    srcs = [
        "streams.cc",
    ],
    hdrs = [
        "streams.h",
    ],
    deps = [
        "//visionai/algorithms/media/util:register_plugins_for_sdk",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams:ingester",
        "//visionai/streams/client:control",
        "//visionai/streams/client:event_update_receiver",
        "//visionai/streams/client:packet_receiver",
        "//visionai/streams/client:packet_sender",
        "//visionai/streams/client:resource_util",
        "//visionai/streams/packet",
        "//visionai/streams/plugins/captures:file_source_capture",
        "//visionai/streams/plugins/captures:rtsp_capture",
        "//visionai/streams/plugins/event_writers:streams_event_writer",
        "//visionai/streams/plugins/filters:noop_filter",
        "//visionai/util:random_string",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
    ],
)
