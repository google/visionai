# This is the directory for the Streams libraries.

package(default_visibility = [
    "//visionai:__subpackages__",
])

licenses(["notice"])

cc_library(
    name = "worker",
    srcs = ["worker.cc"],
    hdrs = ["worker.h"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "worker_test",
    srcs = ["worker_test.cc"],
    deps = [
        ":worker",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "h264_frame_buffer",
    srcs = ["h264_frame_buffer.cc"],
    hdrs = ["h264_frame_buffer.h"],
    deps = [
        "//visionai/streams/packet",
        "//visionai/types:gstreamer_buffer",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "h264_frame_buffer_test",
    srcs = ["h264_frame_buffer_test.cc"],
    deps = [
        ":h264_frame_buffer",
        "//visionai/types:gstreamer_buffer",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)
