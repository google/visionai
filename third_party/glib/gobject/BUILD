load("//third_party/glib:tools.bzl", "glib_mkenums")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["restricted"])

cc_library(
    name = "gobject",
    srcs = [],
    hdrs = [
        # This header file only includes files from gobject, so it should also be exported by it
        "//third_party/glib/glib:glib-object.h",
        "gvaluecollector.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":gobject_internal",
    ],
)

cc_library(
    name = "gobject_internal",
    srcs = glob(
        ["**/*.c"],
        exclude = [
            "gobject-query.c",
        ],
    ) + [
        # This file is created by a genrule
        "glib-enumtypes.c",
    ],
    copts = [
        "-w",
        "-DGOBJECT_COMPILATION",
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GObject\"'",
    ],
    textual_hdrs = glob(["**/*.h"]) + [
        # This file is created by a genrule
        "glib-enumtypes.h",
    ],
    deps = [
        "//third_party/glib/glib:glib_internal",
        "//third_party/libffi",
        "//third_party/pcre",
    ],
)

cc_binary(
    name = "gobject_query",
    srcs = [
        "gobject-query.c",
    ],
    copts = [
        "-w",
        "-DGOBJECT_COMPILATION",
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GObject\"'",
    ],
    deps = [
        ":gobject",
        "//third_party/glib/glib:glib_internal",
    ],
)

py_binary(
    name = "glib_genmarshal",
    srcs = [
        "glib-genmarshal.py",
    ],
    main = "glib-genmarshal.py",
    python_version = "PY3",
    visibility = ["//third_party/glib:build_tool_users"],
)

py_binary(
    name = "glib_mkenums",
    srcs = [
        "glib-mkenums.py",
    ],
    main = "glib-mkenums.py",
    python_version = "PY3",
    visibility = ["//third_party/glib:build_tool_users"],
)

[
    glib_mkenums(
        name = "glib_mkenums_for_{}".format(target.replace("/", "_").replace(".", "_").replace("-", "_")),
        out = target,
        hdrs = hdrs,
        template = target + ".template",
    )
    for target, hdrs in [
        [
            "glib-enumtypes.h",
            ["//third_party/glib/glib:gunicode.h"],
        ],
        [
            "glib-enumtypes.c",
            ["//third_party/glib/glib:gunicode.h"],
        ],
    ]
]
