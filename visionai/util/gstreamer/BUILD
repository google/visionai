package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "pipeline_string",
    srcs = ["pipeline_string.cc"],
    hdrs = ["pipeline_string.h"],
    deps = [
        ":constants",
        "//third_party/gstreamer/subprojects/gstreamer:gst",
        "//visionai/algorithms/media/util",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "pipeline_string_test",
    srcs = ["pipeline_string_test.cc"],
    deps = [
        ":pipeline_string",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
    deps = [
        "@com_google_absl//absl/time",
    ],
)
