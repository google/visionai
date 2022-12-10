package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],  # Apache 2.0
)

exports_files([
    "exercise_1min.mp4",
])

filegroup(
    name = "data",
    srcs = [
        "h265.mp4",  # h265 encoded video.
        "page-brin-1-frame.mp4",  # 1 frame video.
        "page-brin-4-frames.mp4",  # 4 frame video.
    ] + glob([
        "encoded-frames/*.264",
        "jpegs/*.jpg",
        "ppms/*.ppm",
        "pngs/*.png",
        "texts/*.pbtxt",
        "texts/*.csv",
    ]),
)

filegroup(
    name = "motion_data",
    srcs = glob(["motion/*.*"]),
)

filegroup(
    name = "person_data",
    srcs = glob(["person/*.*"]),
)

filegroup(
    name = "exercise_video",
    srcs = ["exercise_1min.mp4"],
)
