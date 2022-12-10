"Provides Starlark rules corresponding to tools used in glib's build."

def _glib_genmarshal_impl(ctx):
    "Generate marshalling files (source & header) using glib-genmarshal."
    c_out = ctx.actions.declare_file(ctx.attr.name + ".c")
    h_out = ctx.actions.declare_file(ctx.attr.name + ".h")

    h_args = ctx.actions.args()
    if ctx.attr.prefix:
        h_args.add("--prefix", ctx.attr.prefix)
    h_args.add("--output", h_out)
    h_args.add("--pragma-once")
    h_args.add("--header", ctx.file.src)

    c_args = ctx.actions.args()
    if ctx.attr.prefix:
        c_args.add("--prefix", ctx.attr.prefix)
    c_args.add("--output", c_out)
    c_args.add("--body", ctx.file.src)
    c_args.add("--include-header", h_out)

    ctx.actions.run(
        inputs = [ctx.file.src],
        outputs = [h_out],
        executable = ctx.executable._glib_genmarshal,
        arguments = [h_args],
        mnemonic = "GlibGenmarshal",
        progress_message = "Generating {}".format(h_out.basename),
    )

    ctx.actions.run(
        inputs = [ctx.file.src],
        outputs = [c_out],
        executable = ctx.executable._glib_genmarshal,
        arguments = [c_args],
        mnemonic = "GlibGenmarshal",
        progress_message = "Generating {}".format(c_out.basename),
    )

    return [
        DefaultInfo(files = depset(direct = [c_out, h_out])),
        OutputGroupInfo(
            source_file = [c_out],
            header_file = [h_out],
        ),
    ]

_glib_genmarshal_attrs = {
    "src": attr.label(
        mandatory = True,
        allow_single_file = [".list"],
    ),
    "prefix": attr.string(),
    "_glib_genmarshal": attr.label(
        default = Label("//third_party/glib/gobject:glib_genmarshal"),
        executable = True,
        cfg = "exec",
    ),
}

glib_genmarshal = rule(
    _glib_genmarshal_impl,
    attrs = _glib_genmarshal_attrs,
    output_to_genfiles = True,
)

def _glib_mkenums_impl(ctx):
    "Generate enum classes for glib using glib-mkenums."
    args = ctx.actions.args()
    args.add("--template", ctx.file.template)
    args.add("--output", ctx.outputs.out)
    args.add_all(ctx.files.hdrs)

    outputs = [ctx.outputs.out]
    ctx.actions.run(
        inputs = [ctx.file.template] + ctx.files.hdrs,
        outputs = outputs,
        executable = ctx.executable._glib_mkenums,
        arguments = [args],
        mnemonic = "GlibMkenums",
        progress_message = "Generating {}".format(ctx.outputs.out.basename),
    )

    return [
        DefaultInfo(files = depset(direct = outputs)),
    ]

_glib_mkenums_attrs = {
    "hdrs": attr.label_list(
        mandatory = True,
        allow_files = True,
    ),
    "out": attr.output(mandatory = True),
    "template": attr.label(
        mandatory = True,
        allow_single_file = True,
    ),
    "_glib_mkenums": attr.label(
        default = Label("//third_party/glib/gobject:glib_mkenums"),
        executable = True,
        cfg = "exec",
    ),
}

glib_mkenums = rule(
    _glib_mkenums_impl,
    attrs = _glib_mkenums_attrs,
    output_to_genfiles = True,
)
