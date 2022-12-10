package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "jpeg",
    srcs = [
        "gstjpeg.c",
        "gstjpegdec.c",
        "gstjpegenc.c",
        "gstjpegplugin.c",
    ],
    copts = DEFAULT_COPTS,
    textual_hdrs = [
        "gstjpeg.h",
        "gstjpegenc.h",
        "gstjpegdec.h",
        "gstjpegelements.h",
    ],
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gst_plugins_good:config",
        "//third_party/gstreamer/subprojects/gst_plugins_good/gst_libs/gst",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "@linux_libjpeg//:libjpeg",
    ],
)
