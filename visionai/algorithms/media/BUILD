package(
    default_visibility = [
        "//cloud/ai/visionai:__subpackages__",
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "gstreamer_async_decoder",
    hdrs = [
        "gstreamer_async_decoder.h",
    ],
    deps = [
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/types:gstreamer_buffer",
        "//visionai/types:raw_image",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util:tuple_apply",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
        "@com_google_absl//absl/types:variant",
    ],
)

cc_test(
    name = "gstreamer_async_decoder_test",
    srcs = ["gstreamer_async_decoder_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":gstreamer_async_decoder",
        "//third_party/gstreamer/subprojects/gst_libav",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst/videoparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/codecparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_playback",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/videoconvert",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/jpeg",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//third_party/gstreamer/subprojects/gstreamer:plugins",
        "//visionai/algorithms/media/util",
        "//visionai/algorithms/media/util:test_util",
        "//visionai/testing/status:status_matchers",
        "//visionai/types:gstreamer_buffer",
        "//visionai/types:raw_image",
        "//visionai/util:file_helpers",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "gstreamer_async_jpeg_encoder",
    hdrs = ["gstreamer_async_jpeg_encoder.h"],
    deps = [
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util:tuple_apply",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "gstreamer_async_jpeg_encoder_test",
    srcs = ["gstreamer_async_jpeg_encoder_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":gstreamer_async_decoder",
        ":gstreamer_async_jpeg_encoder",
        "//third_party/gstreamer/subprojects/gst_libav",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst/videoparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/codecparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_playback",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videoscale",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/videoconvert",
        "//third_party/gstreamer/subprojects/gst_plugins_good:multifile",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/jpeg",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//third_party/gstreamer/subprojects/gstreamer:plugins",
        "//visionai/algorithms/media/util",
        "//visionai/algorithms/media/util:test_util",
        "//visionai/testing/status:status_matchers",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:file_helpers",
        "//visionai/util:file_path",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "gstreamer_video_writer",
    srcs = ["gstreamer_video_writer.cc"],
    hdrs = ["gstreamer_video_writer.h"],
    deps = [
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/algorithms/media/util",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/types:gstreamer_buffer",
        "//visionai/types:raw_image",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "gstreamer_video_writer_test",
    srcs = ["gstreamer_video_writer_test.cc"],
    data = ["//visionai/testing/testdata/media:exercise_1min.mp4"],
    deps = [
        ":gstreamer_video_writer",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",  # buildcleaner: keep
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/algorithms/media/util:gstreamer_registry",
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/types:gstreamer_buffer",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "gstreamer_async_motion_decoder",
    hdrs = ["gstreamer_async_motion_decoder.h"],
    deps = [
        "//visionai/algorithms/media/util:gstreamer_runner",
        "//visionai/algorithms/media/util:type_util",
        "//visionai/types:gstreamer_buffer",
        "//visionai/types:motion_vector",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util:tuple_apply",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/synchronization",
        "@com_google_absl//absl/time",
    ],
)

cc_test(
    name = "gstreamer_async_motion_decoder_test",
    srcs = ["gstreamer_async_motion_decoder_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":gstreamer_async_motion_decoder",
        "//visionai/algorithms/media/util",
        "//visionai/algorithms/media/util:gstreamer_registry",
        "//visionai/algorithms/media/util:test_util",
        "//visionai/types:gstreamer_buffer",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)
