load("//third_party/glib:tools.bzl", "glib_mkenums")

package(
    default_visibility = ["//visibility:public"],
)

licenses(["restricted"])

GIO_MODULE_DIR = "-DGIO_MODULE_DIR='\"/usr/lib/glib/gio/modules\"'"

GIO_ENUMTYPES_HDRS = glob(
    ["**/*.h"],
    exclude = [
        "gdbusauthmechanism.h",
        "glocalfileinfo.h",
        "xdgmime/xdgmimeglob.h",
    ],
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
            "gioenumtypes.h",
            GIO_ENUMTYPES_HDRS,
        ],
        [
            "gioenumtypes.c",
            GIO_ENUMTYPES_HDRS,
        ],
    ]
]

py_binary(
    name = "gdbus_codegen",
    srcs = glob(["gdbus-2.0/codegen/*.py"]),
    main = "gdbus-2.0/codegen/gdbus-codegen.py",
    python_version = "PY3",
)

[
    genrule(
        name = "gdbus_codegen_for_{}".format(out.replace("-", "_")),
        srcs = srcs,
        outs = [
            "{}.c".format(out),
            "{}.h".format(out),
        ],
        cmd = ("$(location :gdbus_codegen) --interface-prefix {prefix} " +
               "--output-directory $$(dirname $(location {out}.c)) " +
               "--generate-c-code {out} --c-namespace {namespace} {srcs}").format(
            srcs = " ".join(["$(location {})".format(s) for s in srcs]),
            out = out,
            namespace = namespace,
            prefix = prefix,
        ),
        tools = [":gdbus_codegen"],
    )
    for prefix, out, namespace, srcs in [
        [
            "org.freedesktop.portal.",
            "xdp-dbus",
            "GXdp",
            glob(["org.freedesktop.portal.*.xml"]),
        ],
        [
            "org.",
            "gdbus-daemon-generated",
            "_G",
            ["dbus-daemon.xml"],
        ],
    ]
]

py_binary(
    name = "data_to_c",
    srcs = ["data-to-c.py"],
    main = "data-to-c.py",
    python_version = "PY3",
)

genrule(
    name = "data_to_c_for_glib_gconstructor_h",
    srcs = ["//third_party/glib/glib:gconstructor.h"],
    outs = ["gconstructor_as_data.h"],
    cmd = "$(location :data_to_c) $(location //third_party/glib/glib:gconstructor.h) gconstructor_code $@",
    tools = [":data_to_c"],
)

cc_library(
    name = "gio",
    srcs = [],
    hdrs = [
        "gio.h",
        "gnetworking.h",
        "gunixfdlist.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_library(
    name = "gio_internal",
    srcs = glob(
        ["**/*.c"],
        exclude = [
            "gapplication-tool.c",
            "gdbus-tool.c",
            "gio-tool*.c",
            "gio-querymodules.c",
            "glib-compile-schemas.c",
            "glib-compile-resources.c",
            "gresource-tool.c",
            "gsettings-tool.c",
        ],
    ) + [
        # These files are created by genrules
        "xdp-dbus.c",
        "gdbus-daemon-generated.c",
        "gioenumtypes.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
        "-DHAVE_CONFIG_H",
        "-DXDG_PREFIX=_gio_xdg",
        "-DLOCALSTATEDIR='\"\"'",
    ],
    linkopts = [
        "-lresolv",
    ],
    textual_hdrs = [
        "strinfo.c",
        # These files are created by genrules
        "xdp-dbus.h",
        "gdbus-daemon-generated.h",
        "gioenumtypes.h",
    ] + glob(
        ["**/*.h"],
    ),
    deps = [
        "//third_party/glib/glib:glib_internal",
        "//third_party/glib/gmodule:gmodule_internal",
        "//third_party/glib/gobject:gobject_internal",
        "@zlib",
    ],
)

cc_binary(
    name = "gio_querymodules",
    srcs = [
        "gio-querymodules.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_library(
    name = "gvdb_builder",
    srcs = [
        "gvdb/gvdb-builder.c",
    ],
    hdrs = ["gvdb/gvdb-builder.h"],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
        "//third_party/glib/gobject:gobject_internal",
    ],
)

cc_binary(
    name = "glib_compile_schemas",
    srcs = [
        "glib-compile-schemas.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    visibility = ["//third_party/glib:build_tool_users"],
    deps = [
        ":gio_internal",
        ":gvdb_builder",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_binary(
    name = "glib_compile_resources",
    srcs = [
        "gconstructor_as_data.h",
        "glib-compile-resources.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    visibility = ["//third_party/glib:build_tool_users"],
    deps = [
        ":gio_internal",
        ":gvdb_builder",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_binary(
    name = "gapplication",
    srcs = [
        "gapplication-tool.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_binary(
    name = "gio_tool",
    srcs = [
        "gio-tool.c",
    ] + glob(
        ["gio-tool-*.c"],
    ),
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_binary(
    name = "gsettings",
    srcs = [
        "gsettings-tool.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_binary(
    name = "gdbus",
    srcs = [
        "gdbus-tool.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
    ],
)

cc_binary(
    name = "gresource",
    srcs = [
        "gresource-tool.c",
    ],
    copts = [
        "-w",
        "-DGIO_COMPILATION",
        GIO_MODULE_DIR,
        "-DG_DISABLE_CAST_CHECKS",
        "-DG_LOG_DOMAIN='\"GLib-GIO\"'",
    ],
    deps = [
        ":gio_internal",
        "//third_party/glib/glib:glib_internal",
        "//third_party/glib/gobject:gobject_internal",
    ],
)
