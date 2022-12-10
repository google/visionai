# This is the directory for the customer facing C++ public interfaces.

load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

proto_library(
    name = "hello_proto",
    srcs = ["hello.proto"],
)

cc_proto_library(
    name = "hello_cc_proto",
    deps = [":hello_proto"],
)

cc_binary(
    name = "sender_app",
    srcs = ["sender_app.cc"],
    deps = [
        ":hello_cc_proto",
        "//visionai/public:streams",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_binary(
    name = "receiver_app",
    srcs = ["receiver_app.cc"],
    deps = [
        ":hello_cc_proto",
        "//visionai/public:streams",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
    ],
)

cc_binary(
    name = "ingest_mp4_app",
    srcs = ["ingest_mp4_app.cc"],
    deps = [
        "//visionai/public:streams",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
    ],
)

cc_binary(
    name = "ingest_rtsp_app",
    srcs = ["ingest_rtsp_app.cc"],
    deps = [
        "//visionai/public:streams",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/flags:usage",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/time",
    ],
)
