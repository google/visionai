package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "videoparsers",
    srcs = [
        "dirac_parse.c",
        "gstav1parse.c",
        "gstdiracparse.c",
        "gsth263parse.c",
        "gsth264parse.c",
        "gsth265parse.c",
        "gstjpeg2000parse.c",
        "gstmpeg4videoparse.c",
        "gstmpegvideoparse.c",
        "gstpngparse.c",
        "gstvc1parse.c",
        "gstvideoparserselement.c",
        "gstvideoparseutils.c",
        "gstvp9parse.c",
        "h263parse.c",
        "plugin.c",
    ],
    hdrs = [
        "dirac_parse.h",
        "gstav1parse.h",
        "gstdiracparse.h",
        "gsth263parse.h",
        "gsth264parse.h",
        "gsth265parse.h",
        "gstjpeg2000parse.h",
        "gstmpeg4videoparse.h",
        "gstmpegvideoparse.h",
        "gstpngparse.h",
        "gstvc1parse.h",
        "gstvideoparserselements.h",
        "gstvideoparseutils.h",
        "gstvp9parse.h",
        "h263parse.h",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_bad:config",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/codecparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
