package(
    default_visibility = ["//visibility:public"],
)

licenses(["restricted"])

cc_library(
    name = "glib",
    srcs = [],
    hdrs = [
        "gi18n.h",
        "glib.h",
        "glib-unix.h",
        "gprintf.h",
        "gstdio.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":glib_internal",
    ],
)

cc_library(
    name = "glib_internal",
    srcs = glob(
        [
            "**/*.c",
        ],
        exclude = [
            "gtester.c",
        ],
    ),
    copts = [
        "-w",
        "-DGLIB_COMPILATION",
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib\"'",
        "-DLIBDIR='\"/usr/lib/glib/\"'",
    ],
    linkopts = [
        "-lpthread",
        "-lrt",
    ],
    # When compiling under GRTEv4, certain features need to be turned off.
    textual_hdrs = glob(
        [
            "**/*.h",
        ],
    ),
    deps = [
        "//third_party/pcre",
    ],
)

exports_files([
    "gunicode.h",
    "gconstructor.h",
    "glib-object.h",
])

cc_binary(
    name = "gtester",
    srcs = [
        "config.h",
        "gtester.c",
    ],
    copts = [
        "-w",
        "-DGLIB_COMPILATION",
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib\"'",
    ],
    deps = [
        ":glib_internal",
        "//third_party/pcre",
    ],
)
