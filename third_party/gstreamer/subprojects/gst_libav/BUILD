# This is the BUILD for gst-libav.
load("//third_party/gstreamer:tools.bzl", "gen_config_file")

package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

gen_config_file(
    name = "gen_config",
    out = "config.h",
    defs = {
        "GST_PACKAGE_NAME": '"GStreamer FFMPEG Plug-ins git"',
        "GST_PACKAGE_ORIGIN": '"Unknown package origin"',
        "HAVE_UNISTD_H": "1",
        "PACKAGE": '"gst-libav"',
        "PACKAGE_VERSION": '"1.19.3"',
        "LIBAV_SOURCE": '"system install"',
    },
)

DEFAULT_COPTS = [
    "-w",
    "-Wno-implicit-function-declaration",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "gst_libav",
    srcs = [
        "ext/libav/gst-motion-meta.c",
        "ext/libav/gstav.c",
        "ext/libav/gstavauddec.c",
        "ext/libav/gstavaudenc.c",
        "ext/libav/gstavcfg.c",
        "ext/libav/gstavcodecmap.c",
        "ext/libav/gstavdeinterlace.c",
        "ext/libav/gstavdemux.c",
        "ext/libav/gstavmux.c",
        "ext/libav/gstavprotocol.c",
        "ext/libav/gstavutils.c",
        "ext/libav/gstavviddec.c",
        "ext/libav/gstavvidenc.c",
    ],
    hdrs = [
        "ext/libav/gstav.h",
        "ext/libav/gstavauddec.h",
        "ext/libav/gstavaudenc.h",
        "ext/libav/gstavcfg.h",
        "ext/libav/gstavcodecmap.h",
        "ext/libav/gstavprotocol.h",
        "ext/libav/gstavutils.h",
        "ext/libav/gstavviddec.h",
        "ext/libav/gstavvidenc.h",
        "ext/libav/gst-motion-meta.h",
        # Generated code.
        "config.h",
    ],
    copts = DEFAULT_COPTS,
    textual_hdrs = [],
    deps = [
        "//third_party/ffmpeg:gen_ffmpeg_lib",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "avvidenc_element_test",
    srcs = [
        "tests/check/elements/avvidenc.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        ":gst_libav",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)
