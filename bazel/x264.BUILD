cc_library(
    name = "x264",
    srcs = glob(
        [
            "lib/x86_64-linux-gnu/libx264.so",
        ],
    ),
    hdrs = glob([
        "include/x264_config.h",
        "include/x264.h",
    ]),
    includes = ["include"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
