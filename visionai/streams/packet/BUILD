package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "packet",
    srcs = [
        "packet.cc",
    ],
    hdrs = [
        "packet.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//visionai/streams/packet/packet_codecs",
        "//visionai/util:time_util",
        "//visionai/util/status:status_macros",
        "@com_github_googleapis_googleapis//google/cloud/visionai/v1:visionai_cc_proto",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_absl//absl/time",
    ],
)
