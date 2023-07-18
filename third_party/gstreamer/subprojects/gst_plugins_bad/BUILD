load("//third_party/gstreamer:tools.bzl", "gen_config_file", "gnome_mkenums_simple")

package(
    default_visibility = ["//visibility:public"],
    features = ["-layering_check"],
)

licenses(["restricted"])

gen_config_file(
    name = "gen_config",
    out = "config.h",
    defs = {
        "DEFAULT_VIDEOSRC": '"v4l2src"',
        "ENABLE_NLS": "1",
        "GETTEXT_PACKAGE": '"gst-plugins-bad-1.0"',
        "GST_API_EXPORT": 'extern __attribute__ ((visibility ("default")))',
        "GST_API_VERSION": '"1.0"',
        "GST_ENABLE_EXTRA_CHECKS": "",
        "GST_LICENSE": '"LGPL"',
        "GST_PACKAGE_NAME": '"GStreamer Bad Plug-ins git"',
        "GST_PACKAGE_ORIGIN": '"Unknown package origin"',
        "HAVE_DCGETTEXT": "1",
        "HAVE_DLFCN_H": "1",
        "HAVE_FCNTL_H": "1",
        "HAVE_GETPAGESIZE": "",
        "HAVE_GETRUSAGE": "",
        "HAVE_GMTIME_R": "1",
        "HAVE_MAKEDEV_IN_MKDEV": "0",
        "HAVE_MAKEDEV_IN_SYSMACROS": "1",
        "HAVE_MAKEDEV_IN_TYPES": "0",
        "HAVE_MEMFD_CREATE": "1",
        "HAVE_MEMORY_H": "1",
        "HAVE_MMAP": "1",
        "HAVE_NETINET_IN_H": "1",
        "HAVE_NETINET_IP_H": "1",
        "HAVE_NETINET_TCP_H": "1",
        "HAVE_OPENSSL": "1",
        "HAVE_OGG": "",
        #"HAVE_ORC": "1", b/226934292
        "HAVE_PIPE2": "1",
        "HAVE_PTHREAD_H": "1",
        "HAVE_STDINT_H": "1",
        "HAVE_STDLIB_H": "1",
        "HAVE_STRINGS_H": "1",
        "HAVE_STRING_H": "1",
        "HAVE_SYS_PARAM_H": "1",
        "HAVE_SYS_SOCKET_H": "1",
        "HAVE_SYS_STAT_H": "1",
        "HAVE_SYS_TYPES_H": "1",
        "HAVE_SYS_UTSNAME_H": "1",
        "HAVE_UNISTD_H": "1",
        "HAVE_X11": "",
        "LIBDIR": "/usr/local/lib/x86_64-linux-gnu",
        "LOCALEDIR": '"/usr/local/share/locale"',
        "PACKAGE": '"gst-plugins-bad"',
        "PACKAGE_BUGREPORT": '"https://gitlab.freedesktop.org/gstreamer/gst-plugins-bad/issues/new"',
        "PACKAGE_NAME": '"GStreamer Bad Plug-ins"',
        "PACKAGE_VERSION": '"1.19.3.1"',
        "SIZEOF_CHAR": "1",
        "SIZEOF_INT": "4",
        "SIZEOF_LONG": "8",
        "SIZEOF_SHORT": "2",
        "SIZEOF_VOIDP": "8",
        "VERSION": '"1.19.3.1"',
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
    name = "plugin_mpegtsmux",
    srcs = glob([
        "gst/mpegtsmux/*.c",
        "gst/mpegtsmux/tsmux/*.c",
    ]),
    copts = DEFAULT_COPTS,
    textual_hdrs = glob([
        "gst/mpegtsmux/*.h",
        "gst/mpegtsmux/tsmux/*.h",
    ]),
    deps = [
        ":config",
        ":lib_mpegts",
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst/videoparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_audio",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_tag",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "lib_uridownloader",
    srcs = glob(["gst_libs/gst/uridownloader/*.c"]),
    hdrs = glob(["gst_libs/gst/uridownloader/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_library(
    name = "lib_gst",
    copts = DEFAULT_COPTS,
    textual_hdrs = glob(["gst_libs/gst/*.h"]),
    deps = ["//third_party/glib/glib"],
)

cc_library(
    name = "lib_adaptivedemux",
    srcs = glob(["gst_libs/gst/adaptivedemux/*.c"]),
    hdrs = glob(["gst_libs/gst/adaptivedemux/*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        ":config",
        ":lib_gst",
        ":lib_uridownloader",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

gnome_mkenums_simple(
    name = "gstmpegts-enumtypes",
    body_prefix = '#ifdef HAVE_CONFIG_H\n#include "third_party/gstreamer/subprojects/gst_plugins_bad/config.h"\n#endif\n',
    decorator = "GST_MPEGTS_API",
    hdr_out = "gst_libs/gst/mpegts/gstmpegts-enumtypes.h",
    header_prefix = '#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/mpegts/mpegts-prelude.h"',
    sources = glob(["gst_libs/gst/mpegts/*.h"]),
    src_out = "gst_libs/gst/mpegts/gstmpegts-enumtypes.c",
)

cc_library(
    name = "lib_mpegts",
    srcs = glob(["gst_libs/gst/mpegts/*.c"]) + ["gst_libs/gst/mpegts/gstmpegts-enumtypes.c"],
    copts = DEFAULT_COPTS,
    textual_hdrs = glob(["gst_libs/gst/mpegts/*.h"]) + ["gst_libs/gst/mpegts/gstmpegts-enumtypes.h"],
    deps = [
        ":config",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
