package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_binary(
    name = "oc_visualization_app",
    srcs = ["oc_visualization_app.cc"],
    linkopts = [
        "-Wl,--dynamic-linker=/lib64/ld-linux-x86-64.so.2",
        "-Wl,-rpath=/usr/lib/x86_64-linux-gnu:/lib/x86_64-linux-gnu",
    ],
    deps = [
        ":receive_cat_visual_tool",
        ":render_utils",
        "//visionai/algorithms/media/util:gstreamer_registry",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/streams/apps:flags",
        "//visionai/streams/client:event_update",
        "//visionai/streams/client:event_update_receiver",
        "//visionai/streams/client:resource_util",
        "//visionai/util/status:status_macros",
        "//visionai/util/thread:sync_queue",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/cleanup",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@linux_opencv//:opencv",
    ],
)

cc_library(
    name = "receive_cat_visual_tool",
    srcs = ["receive_cat_visual_tool.cc"],
    hdrs = ["receive_cat_visual_tool.h"],
    deps = [
        "//visionai/algorithms/media:gstreamer_async_decoder",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/streams/client:descriptors",
        "//visionai/streams/client:packet_receiver",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:time_util",
        "//visionai/util/status:status_macros",
        "//visionai/util/thread:sync_queue",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "render_utils",
    srcs = ["render_utils.cc"],
    hdrs = ["render_utils.h"],
    deps = [
        "//visionai/util:time_util",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@linux_opencv//:opencv",
    ],
)

cc_test(
    name = "render_utils_test",
    srcs = ["render_utils_test.cc"],
    deps = [
        ":render_utils",
        "//visionai/util:time_util",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
        "@com_google_protobuf//:protobuf",
        "@linux_opencv//:opencv",
    ],
)
