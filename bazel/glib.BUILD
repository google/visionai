load("@bazel_skylib//rules:native_binary.bzl", "native_binary")

cc_library(
    name = "glib",
    srcs = glob([
        "lib/x86_64-linux-gnu/libglib-2.0.so",
    ]),
    hdrs = glob([
        "include/glib/glib/*.h",
        "include/glib/gio/*.h",
        "include/glib/gobject/*.h",
        "include/glib/gmodule/*.h",
        "include/glib/*.h",
    ]),
    includes = ["include/glib"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gmodule",
    srcs = glob([
        "lib/x86_64-linux-gnu/libgmodule-2.0.so",
    ]),
    hdrs = glob([
        "include/glib/glib/*.h",
        "include/glib/gio/*.h",
        "include/glib/gobject/*.h",
        "include/glib/gmodule/*.h",
        "include/glib/*.h",
    ]),
    includes = ["include/glib"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gio",
    srcs = glob([
        "lib/x86_64-linux-gnu/libgio-2.0.so",
    ]),
    hdrs = glob([
        "include/glib/glib/*.h",
        "include/glib/gio/*.h",
        "include/glib/gobject/*.h",
        "include/glib/gmodule/*.h",
        "include/glib/*.h",
    ]),
    includes = ["include/glib"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

cc_library(
    name = "gobject",
    srcs = glob([
        "lib/x86_64-linux-gnu/libgobject-2.0.so",
    ]),
    hdrs = glob([
        "include/glib/glib/*.h",
        "include/glib/gio/*.h",
        "include/glib/gobject/*.h",
        "include/glib/gmodule/*.h",
        "include/glib/*.h",
    ]),
    includes = ["include/glib"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)

native_binary(
    name = "glib-mkenums",
    src = "bin/glib-mkenums",
    out = "glib-mkenums",
    visibility = ["//visibility:public"],
)