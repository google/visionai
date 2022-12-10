package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "typefind",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/glib",
        "//third_party/gstreamer/subprojects/gst_plugins_base:config",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_common",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_pbutils",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
