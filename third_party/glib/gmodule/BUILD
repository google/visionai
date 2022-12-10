package(
    default_visibility = ["//visibility:public"],
)

licenses(["restricted"])

cc_library(
    name = "gmodule",
    srcs = [],
    hdrs = [
        "gmodule.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":gmodule_internal",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_library(
    name = "gmodule_internal",
    srcs = [
        "gmodule.c",
        "gmoduleconf.h",
    ],
    copts = [
        "-w",
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_DISABLE_DEPRECATED",
        "-DG_LOG_DOMAIN='\"GModule\"'",
    ],
    linkopts = [
        "-ldl",
    ],
    textual_hdrs = [
        "gmodule-dl.c",
        "gmodule.h",
    ],
    deps = [
        "//third_party/glib/glib:glib_internal",
        "//third_party/pcre",
    ],
)
