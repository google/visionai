package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

DEFAULT_COPTS = [
    "-w",
    "-DHAVE_CONFIG_H",
]

cc_library(
    name = "soap",
    srcs = [
        "gstsoup.c",
        "gstsoupelement.c",
        "gstsouphttpclientsink.c",
        "gstsouphttpsrc.c",
        "gstsouploader.c",
        "gstsouputils.c",
    ],
    hdrs = [
        "gstsoupelements.h",
        "gstsouphttpclientsink.h",
        "gstsouphttpsrc.h",
        "gstsouploader.h",
        "gstsouputils.h",
    ],
    copts = DEFAULT_COPTS,
    deps = [
        "//third_party/glib/gio",
        "//third_party/glib/glib",
        "//third_party/glib/gmodule",
        "//third_party/glib/gobject",
        "//third_party/gstreamer/subprojects/gst_plugins_base:gst_libs_tag",
        "//third_party/gstreamer/subprojects/gst_plugins_good:config",
        "//third_party/gstreamer/subprojects/gst_plugins_good/ext/soup/stub",
        "//third_party/gstreamer/subprojects/gst_plugins_good/gst_libs/gst",
        "//third_party/gstreamer/subprojects/gstreamer:base",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
    ],
)
