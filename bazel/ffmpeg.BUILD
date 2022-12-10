cc_library(
    name = "ffmpeg",
    srcs = glob(
        [
            "lib/x86_64-linux-gnu/libavcodec.so",
            "lib/x86_64-linux-gnu/libavdevice.so",
            "lib/x86_64-linux-gnu/libavfilter.so",
            "lib/x86_64-linux-gnu/libavformat.so",
            "lib/x86_64-linux-gnu/libavresample.so",
            "lib/x86_64-linux-gnu/libavutil.so",
            "lib/x86_64-linux-gnu/libpostproc.so",
            "lib/x86_64-linux-gnu/libswresample.so",
            "lib/x86_64-linux-gnu/libswscale.so",
        ],
    ),
    hdrs = glob([
        "include/x86_64-linux-gnu/libavcodec/*.h",
        "include/x86_64-linux-gnu/libavformat/*.h",
        "include/x86_64-linux-gnu/libavutil/*.h",
        "include/x86_64-linux-gnu/libswresample/*.h",
        "include/x86_64-linux-gnu/libswscale/*.h",
    ]),
    includes = ["include/x86_64-linux-gnu/"],
    linkstatic = 1,
    visibility = ["//visibility:public"],
)
