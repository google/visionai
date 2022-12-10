package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "codecparsers",
    srcs = [
        "gstav1parser.c",
        "gsth264parser.c",
        "gsth265parser.c",
        "gstjpeg2000sampling.c",
        "gstjpegparser.c",
        "gstmpeg4parser.c",
        "gstmpegvideometa.c",
        "gstmpegvideoparser.c",
        "gstvc1parser.c",
        # Skipping vp8parser related code since it depends dboolhuff that has unclear license situation.
        #"gstvp8parser.c",
        #"gstvp8rangedecoder.c",
        "gstvp9parser.c",
        "nalutils.c",
        "parserutils.c",
        #"vp8utils.c",
        "vp9utils.c",
    ],
    hdrs = [
        "codecparsers-prelude.h",
        "gstav1parser.h",
        "gsth264parser.h",
        "gsth265parser.h",
        "gstjpeg2000sampling.h",
        "gstjpegparser.h",
        "gstmpeg4parser.h",
        "gstmpegvideometa.h",
        "gstmpegvideoparser.h",
        "gstvc1parser.h",
        #"gstvp8parser.h",
        #"gstvp8rangedecoder.h",
        "gstvp9parser.h",
        "nalutils.h",
        "parserutils.h",
        #"vp8utils.h",
        "vp9utils.h",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gst_plugins_bad:config",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
