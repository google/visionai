package(default_visibility = ["//visibility:public"])

licenses(["restricted"])

cc_library(
    name = "gst",
    hdrs = [
        "gettext.h",
        "glib-compat-private.h",
        "gst-i18n-plugin.h",
    ],
    copts = ["-DGETTEXT_PACKAGE=\\\"gst-plugins-good-1.0\\\""],
    deps = ["//third_party/glib/glib"],
)
