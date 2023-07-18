load("//third_party/gstreamer:tools.bzl", "gen_config_file")

package(
    default_visibility = ["//visibility:public"],
    features = ["-layering_check"],
)

licenses(["restricted"])

gen_config_file(
    name = "gen_config",
    out = "config.h",
    defs = {
        "ENABLE_NLS": 1,
        "GETTEXT_PACKAGE": '"gst-plugins-good-1.0"',
        "GST_LICENSE": '"LGPL"',
        "GST_PACKAGE_NAME": '"GStreamer Good Plug-ins git"',
        "GST_PACKAGE_ORIGIN": '"Unknown package origin"',
        "GST_V4L2_ENABLE_PROBE": 1,
        "HAVE_ASINH": 1,
        "HAVE_BZ2": 1,
        "HAVE_CLOCK_GETTIME": 1,
        "HAVE_COSH": 1,
        "HAVE_CPU_X86_64": 1,
        "HAVE_DCGETTEXT": 1,
        "HAVE_DLFCN_H": 1,
        "HAVE_FCNTL_H": 1,
        "HAVE_GCC_ASM": 1,
        "HAVE_GETPAGESIZE": 1,
        "HAVE_GMTIME_R": 1,
        "HAVE_GST_V4L2": 1,
        "HAVE_GUDEV": 1,
        "HAVE_INTTYPES_H": 1,
        "HAVE_IOS": 1,
        "HAVE_ISINF": 1,
        "HAVE_LIBV4L2": 1,
        "HAVE_MEMORY_H": 1,
        "HAVE_MMAP": 1,
        "HAVE_ORC": 1,
        "HAVE_OSS_INCLUDE_IN_SYS": 1,
        "HAVE_RTLD_NOLOAD": 1,
        "HAVE_SINH": 1,
        "HAVE_STDINT_H": 1,
        "HAVE_STDLIB_H": 1,
        "HAVE_STRINGS_H": 1,
        "HAVE_STRING_H": 1,
        "HAVE_SYS_IOCTL_H": 1,
        "HAVE_SYS_PARAM_H": 1,
        "HAVE_SYS_SOCKET_H": 1,
        "HAVE_SYS_STAT_H": 1,
        "HAVE_SYS_TIME_H": 1,
        "HAVE_SYS_TYPES_H": 1,
        "HAVE_UNISTD_H": 1,
        "HAVE_ZLIB": 1,
        "LOCALEDIR": '"/usr/local/share/locale"',
        "PACKAGE": '"gst-plugins-good"',
        "PACKAGE_VERSION": '"1.19.3"',
        "SIZEOF_CHAR": 1,
        "SIZEOF_INT": 4,
        "SIZEOF_LONG": 8,
        "SIZEOF_OFF_T": 8,
        "SIZEOF_SHORT": 2,
        "SIZEOF_VOIDP": 8,
        "VERSION": '"1.19.3"',
    },
)

cc_library(
    name = "config",
    hdrs = ["config.h"],
)

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "isomp4",
    srcs = glob(
        ["gst/isomp4/*.c"],
        exclude = ["gst/isomp4/gstqtmux-doc.c"],
    ),
    copts = DEFAULT_COPTS,
    textual_hdrs = glob(
        ["gst/isomp4/*.h"],
        exclude = ["gst/isomp4/gstqtmux-doc.h"],
    ),
    deps = [
        ":config",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_riff",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtp",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_tag",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_good/gst_libs/gst",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "@zlib",
    ],
)

cc_library(
    name = "rtp",
    srcs = glob(["gst/rtp/*.c"]),
    copts = DEFAULT_COPTS + [
        "-Dvp8_norm=gst_rtpvp8_vp8_norm",
        "-Dvp8dx_start_decode=gst_rtpvp8_vp8dx_start_decode",
        "-Dvp8dx_bool_decoder_fill=gst_rtpvp8_vp8dx_bool_decoder_fill",
    ],
    textual_hdrs = glob(["gst/rtp/*.h"]),
    deps = [
        ":config",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtp",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_tag",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "rtpmanager",
    srcs = glob(["gst/rtpmanager/*.c"]),
    copts = DEFAULT_COPTS,
    textual_hdrs = glob(["gst/rtpmanager/*.h"]),
    deps = [
        ":config",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtp",
        "//third_party/gstreamer/subprojects/gst_plugins_good/gst_libs/gst",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//third_party/gstreamer/subprojects/gstreamer:net",
    ],
)

cc_library(
    name = "rtsp",
    srcs = glob(["gst/rtsp/*.c"]),
    hdrs = glob(["gst/rtsp/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtp",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_rtsp",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_sdp",
        "//third_party/gstreamer/subprojects/gst_plugins_good/gst_libs/gst",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//third_party/gstreamer/subprojects/gstreamer:net",
    ],
)

cc_library(
    name = "matroska",
    srcs = glob(["gst/matroska/*.c"]),
    hdrs = glob(["gst/matroska/*.h"]),
    copts = DEFAULT_COPTS,
    textual_hdrs = [],
    deps = [
        ":config",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_riff",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_tag",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "@bzip2",
        "@zlib",
    ],
)

cc_library(
    name = "multifile",
    srcs = glob(
        ["gst/multifile/*.c"],
        exclude = ["gst/multifile/test-splitmuxpartreader.c"],
    ),
    hdrs = glob(["gst/multifile/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_good/gst_libs/gst",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
