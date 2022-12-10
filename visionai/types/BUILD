package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "gstreamer_buffer",
    hdrs = ["gstreamer_buffer.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "gstreamer_buffer_test",
    srcs = ["gstreamer_buffer_test.cc"],
    deps = [
        ":gstreamer_buffer",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "raw_image",
    srcs = ["raw_image.cc"],
    hdrs = ["raw_image.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "raw_image_test",
    srcs = ["raw_image_test.cc"],
    deps = [
        ":raw_image",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "no_copy",
    hdrs = ["no_copy.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "signal",
    srcs = ["signal.cc"],
    hdrs = ["signal.h"],
    visibility = ["//visibility:public"],
    deps = [
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "signal_test",
    srcs = ["signal_test.cc"],
    deps = [
        ":signal",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "motion_vector",
    hdrs = ["motion_vector.h"],
    visibility = ["//visibility:public"],
    deps = [
    ],
)
