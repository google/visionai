package(default_visibility = ["//visibility:public"])

cc_library(
    name = "bzip2",
    hdrs = [
        "bzlib.h",
    ],
    srcs = [
        "blocksort.c",
        "huffman.c",
        "crctable.c",
        "randtable.c",
        "compress.c",
        "decompress.c",
        "bzlib.c",
        "bzlib_private.h",
    ],
    includes = ["."],
)