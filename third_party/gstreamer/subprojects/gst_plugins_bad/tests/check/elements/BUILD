package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

DEFAULT_COPTS = [
    "-w",
    "-Wno-implicit-function-declaration",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "parser",
    srcs = [
        "parser.c",
    ],
    hdrs = [
        "parser.h",
    ],
    copts = DEFAULT_COPTS,
    textual_hdrs = [],
    deps = [
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "h264parse_element_test",
    srcs = [
        "h264parse.c",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        ":parser",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst/videoparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/codecparsers",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:check",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)

cc_test(
    name = "mpegtsmux_element_test",
    srcs = ["mpegtsmux.c"],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/gstreamer/subprojects/gst_plugins_bad:plugin_mpegtsmux",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_video",
        "//third_party/gstreamer/subprojects/gstreamer:check",
    ],
)
