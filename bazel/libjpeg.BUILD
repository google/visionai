cc_library(
    name = "libjpeg",
    srcs = glob(
        [
            "lib/x86_64-linux-gnu/libjpeg.so",
        ],
    ),
    hdrs = glob([
        "include/jerror.h",
        "include/jmorecfg.h",
        "include/jpegint.h",
        "include/jpeglib.h",
        "include/x86_64-linux-gnu/jconfig.h",
    ]),
    includes = ["include"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
