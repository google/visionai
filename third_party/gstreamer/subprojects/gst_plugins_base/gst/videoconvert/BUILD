package(default_visibility = ["//visibility:public"])

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "videoconvert",
    srcs = ["gstvideoconvert.c"],
    hdrs = ["gstvideoconvert.h"],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
