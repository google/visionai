package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_binary(
    name = "receive_cat_app",
    srcs = ["receive_cat_app.cc"],
    deps = [
        ":flags",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/streams/client:descriptors",
        "//visionai/streams/client:event_update",
        "//visionai/streams/client:event_update_receiver",
        "//visionai/streams/client:packet_receiver",
        "//visionai/streams/client:resource_util",
        "//visionai/util:file_helpers",
        "//visionai/util:random_string",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_binary(
    name = "ingester_app",
    srcs = ["ingester_app.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        "//visionai/algorithms/media/util:register_plugins_for_sdk",
        "//visionai/proto:ingester_config_cc_proto",
        "//visionai/streams:ingester",
        "//visionai/streams/plugins/captures:all_captures",
        "//visionai/streams/plugins/event_writers:all_event_writers",
        "//visionai/streams/plugins/filters:all_filters",
        "//visionai/util:file_helpers",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_protobuf//:protobuf",
    ],
)

cc_library(
    name = "flags",
    srcs = ["flags.cc"],
    hdrs = ["flags.h"],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_binary(
    name = "resource_manager_app",
    srcs = ["resource_manager_app.cc"],
    deps = [
        ":flags",
        "//visionai/proto:cluster_selection_cc_proto",
        "//visionai/streams/client:control",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_protobuf//:protobuf",
    ],
)
