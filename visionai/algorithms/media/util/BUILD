# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = [
        "//cloud/ai/visionai:__subpackages__",
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

ALL_PLUGINS_FOR_SDK = [
    "//third_party/gstreamer/subprojects/gst_libav:gst_libav",
    "//third_party/gstreamer/subprojects/gst_plugins_bad:plugin_mpegtsmux",
    "//third_party/gstreamer/subprojects/gst_plugins_bad/gst/videoparsers",
    "//third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/codecparsers",
    "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
    "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_gio",
    "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_playback",
    "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_rawparse",
    "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videorate",
    "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videoscale",
    "//third_party/gstreamer/subprojects/gst_plugins_base/gst/typefind",
    "//third_party/gstreamer/subprojects/gst_plugins_base/gst/videoconvert",
    "//third_party/gstreamer/subprojects/gst_plugins_good:isomp4",
    "//third_party/gstreamer/subprojects/gst_plugins_good:matroska",
    "//third_party/gstreamer/subprojects/gst_plugins_good:multifile",
    "//third_party/gstreamer/subprojects/gst_plugins_good:rtp",
    "//third_party/gstreamer/subprojects/gst_plugins_good:rtpmanager",
    "//third_party/gstreamer/subprojects/gst_plugins_good:rtsp",
    "//third_party/gstreamer/subprojects/gstreamer:plugins",
]

ALL_PLUGINS = ALL_PLUGINS_FOR_SDK + [
]

cc_library(
    name = "gstreamer_registry",
    srcs = [
        "gstreamer_registry.cc",
    ],
    hdrs = [
        "gstreamer_registry.h",
    ],
    deps = [
        ":util",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
    ] + ALL_PLUGINS,
)

cc_library(
    name = "register_plugins_for_sdk",
    srcs = [
        "register_plugins_for_sdk.cc",
    ],
    hdrs = [
        "register_plugins_for_sdk.h",
    ],
    deps = [
        ":util",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
    ] + ALL_PLUGINS_FOR_SDK,
)

cc_library(
    name = "gstreamer_runner",
    srcs = [
        "gstreamer_runner.cc",
    ],
    hdrs = [
        "gstreamer_runner.h",
    ],
    deps = [
        ":gstreamer_registry",
        ":type_util",
        ":util",
        "//third_party/ffmpeg:gen_ffmpeg_lib",
        "//third_party/gstreamer/subprojects/gst_libav",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:completion_signal",
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
    name = "gstreamer_runner_test",
    srcs = ["gstreamer_runner_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":gstreamer_runner",
        ":test_util",
        ":type_util",
        ":util",
        "//third_party/gstreamer/subprojects/gst_libav",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_playback",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/videoconvert",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/jpeg",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//third_party/gstreamer/subprojects/gstreamer:plugins",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:file_helpers",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "util",
    srcs = [
        "util.cc",
    ],
    hdrs = [
        "util.h",
    ],
    deps = [
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "type_util",
    srcs = [
        "type_util.cc",
    ],
    hdrs = [
        "type_util.h",
    ],
    deps = [
        ":util",
        "//third_party/ffmpeg:gen_ffmpeg_lib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/types:gstreamer_buffer",
        "//visionai/types:motion_vector",
        "//visionai/types:raw_image",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "type_util_test",
    srcs = ["type_util_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":gstreamer_runner",
        ":type_util",
        ":util",
        "//third_party/ffmpeg:gen_ffmpeg_lib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_playback",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/videoconvert",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/jpeg",
        "//third_party/gstreamer/subprojects/gstreamer:plugins",
        "//visionai/types:motion_vector",
        "//visionai/types:raw_image",
        "//visionai/util:file_helpers",
        "//visionai/util:producer_consumer_queue",
        "//visionai/util/status:status_macros",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "test_util",
    srcs = ["test_util.cc"],
    hdrs = ["test_util.h"],
    deps = [
        "//visionai/types:gstreamer_buffer",
        "//visionai/util:file_helpers",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status:statusor",
    ],
)

cc_library(
    name = "codec_validator",
    srcs = ["codec_validator.cc"],
    hdrs = ["codec_validator.h"],
    deps = [
        ":gstreamer_runner",
        ":util",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/types:gstreamer_buffer",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "codec_validator_test",
    srcs = ["codec_validator_test.cc"],
    data = ["//visionai/testing/testdata/media:data"],
    deps = [
        ":codec_validator",
        ":gstreamer_registry",
        "//visionai/util:file_path",
        "@com_google_absl//absl/debugging:leak_check",
        "@com_google_googletest//:gtest_main",
    ],
)
