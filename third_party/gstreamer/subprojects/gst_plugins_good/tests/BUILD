# Unit tests for gst_plugs_good subproject

package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

filegroup(
    name = "testdata",
    srcs = [
        "files/h264.rtp",
        "files/h265.rtp",
        "files/image.jpg",
        "files/pinknoise-vorbis.mkv",
    ],
)

DEFAULT_COPTS = [
    "-w",
    "-Wno-implicit-function-declaration",
    "-DHAVE_CONFIG_H",
    "-DGST_TEST_FILES_PATH=\\\"third_party/gstreamer/subprojects/gst_plugins_good/tests/files\\\"",
]

cc_test(
    name = "element_qtmux_test",
    srcs = [
        "check/elements/qtmux.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_good:config",
        "//third_party/gstreamer/subprojects/gst_plugins_good:isomp4",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_qtdemux_test",
    srcs = [
        "check/elements/qtdemux.c",
        "check/elements/qtdemux.h",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_good:config",
        "//third_party/gstreamer/subprojects/gst_plugins_good:isomp4",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_jpegdec_test",
    srcs = [
        "check/elements/jpegdec.c",
    ],
    copts = DEFAULT_COPTS + [
        "-DGST_TEST_FILES_PATH=\\\"third_party/gstreamer/subprojects/gst_plugins_good/tests/files\\\"",
    ],
    data = [
        ":testdata",
    ],
    env = {
        # Turning on the info logs is a hack to deflake the test.
        # This test is flaky because the static registration of the typefindfunctions takes some
        # time to finish.
        "GST_DEBUG": "*:4",
    },
    deps = [
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_playback",
        "//third_party/gstreamer/subprojects/gst_plugins_base/gst/typefind",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/jpeg",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//third_party/gstreamer/subprojects/gstreamer:plugins",
    ],
)

cc_test(
    name = "element_jpegenc_test",
    srcs = [
        "check/elements/jpegenc.c",
    ],
    copts = DEFAULT_COPTS + [
        "-DGST_TEST_FILES_PATH=\\\"third_party/gstreamer/subprojects/gst_plugins_good/tests/files\\\"",
    ],
    data = [
        ":testdata",
    ],
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/jpeg",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_multifile_test",
    srcs = [
        "check/elements/multifile.c",
    ],
    copts = DEFAULT_COPTS + [
        "-DGST_TEST_FILES_PATH=\\\"third_party/gstreamer/subprojects/gst_plugins_good/tests/files\\\"",
    ],
    data = [
        ":testdata",
    ],
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gst_plugins_good:config",
        "//third_party/gstreamer/subprojects/gst_plugins_good:multifile",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

cc_test(
    name = "element_splitmuxsink_test",
    srcs = [
        "check/elements/splitmuxsink.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_audiotestsrc",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_videotestsrc",
        "//third_party/gstreamer/subprojects/gst_plugins_good:config",
        "//third_party/gstreamer/subprojects/gst_plugins_good:isomp4",
        "//third_party/gstreamer/subprojects/gst_plugins_good:matroska",
        "//third_party/gstreamer/subprojects/gst_plugins_good:multifile",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/jpeg",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)

GST_RTP_PLUGINS_TESTS = [
    #"rtp-payloading", missing gst_plugins_base/ext/vorbis.
    "rtpbin",
    "rtpbin_buffer_list",
    #"rtpcollision", missing audiotestsrc.
    "rtpfunnel",
    "rtph261",
    "rtph263",
    "rtph264",
    "rtph265",
    "rtphdrext-colorspace",
    "rtpjitterbuffer",
    "rtpjpeg",
    "rtpmux",
    "rtpopus",
    "rtpptdemux",
    "rtpred",
    "rtprtx",
    "rtpsession",
    "rtpssrcdemux",
    "rtpst2022-1-fecdec",
    "rtpst2022-1-fecenc",
    "rtpstorage",
    "rtptimerqueue",
    "rtpulpfec",
    "rtpvp8",
    "rtpvp9",
    "matroskaparse",
    "matroskamux",
    "matroskademux",
]

[cc_test(
    name = "element_{}_test".format(function),
    srcs = [
        "check/elements/{}.c".format(function),
        "check/elements/packets.h",
    ],
    copts = DEFAULT_COPTS,
    data = [
        ":testdata",
    ],
    env = {
        "CK_DEFAULT_TIMEOUT": "20",
    },
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_app",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtp",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_base:plugin_app",
        "//third_party/gstreamer/subprojects/gst_plugins_good:config",
        "//third_party/gstreamer/subprojects/gst_plugins_good:matroska",
        "//third_party/gstreamer/subprojects/gst_plugins_good:rtp",
        "//third_party/gstreamer/subprojects/gst_plugins_good:rtpmanager",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//third_party/gstreamer/subprojects/gstreamer:net",
    ],
) for function in GST_RTP_PLUGINS_TESTS]
