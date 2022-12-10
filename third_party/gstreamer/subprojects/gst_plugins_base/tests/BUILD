# Unit tests for gst_plugs_base subproject

package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

filegroup(
    name = "testdata",
    srcs = glob(["files/*"]),
)

DEFAULT_COPTS = [
    "-w",
    "-Wno-implicit-function-declaration",
    "-DHAVE_CONFIG_H",
    "-DGST_TEST_FILES_PATH=\\\"third_party/gstreamer/subprojects/gst_plugins_base/tests/files\\\"",
]

cc_test(
    name = "element_appsrc_test",
    srcs = [
        "check/elements/appsrc.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_common",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "element_appsink_test",
    srcs = [
        "check/elements/appsink.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_common",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "element_typefind_test",
    srcs = [
        "check/gst/typefindfunctions.c",
    ],
    copts = DEFAULT_COPTS,
    data = [":testdata"],
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_common",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/typefind",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_videoconvert_test",
    srcs = ["check/elements/videoconvert.c"],
    copts = DEFAULT_COPTS,
    env = {
        "GST_DEBUG": "2",
    },
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/videoconvert",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_videorate_test",
    srcs = ["check/elements/videorate.c"],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videorate",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "element_videoscale_test",
    srcs = ["check/elements/videoscale.c"],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videoscale",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/videoconvert",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_videotestsrc_test",
    srcs = [
        "check/elements/videotestsrc.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_common",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_rtsp_test",
    srcs = [
        "check/libs/rtsp.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtsp",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_rtsp_connection_test",
    srcs = [
        "check/libs/rtspconnection.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtsp",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_audiodecoder_test",
    srcs = [
        "check/libs/audiodecoder.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_audioencoder_test",
    srcs = [
        "check/libs/audioencoder.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_videodecoder_test",
    srcs = [
        "check/libs/videodecoder.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_videoencoder_test",
    srcs = [
        "check/libs/videoencoder.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_pbutils_test",
    srcs = ["check/libs/pbutils.c"],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

# Disabled rtp tests:
#  "check/libs/rtpbasedepayload.c",
#  "check/libs/rtpbasepayload.c",
#  "check/libs/rtphdrext.c",
cc_test(
    name = "lib_rtp_test",
    srcs = [
        "check/libs/rtp.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtp",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_rtp_meta_test",
    srcs = [
        "check/libs/rtpmeta.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtp",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "lib_sdp_test",
    srcs = [
        "check/libs/sdp.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_sdp",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "pipeline_gio_test",
    srcs = ["check/pipelines/gio.c"],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_gio",  # build_cleaner keep
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_rawvideoparse_test",
    srcs = [
        "check/elements/rawvideoparse.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_common",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_rawparse",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
