package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "ignore_index",
    hdrs = ["ignore_index.h"],
)

cc_library(
    name = "intrinsics",
    hdrs = ["intrinsics.h"],
)

cc_test(
    name = "intrinsics_test",
    size = "small",
    srcs = ["intrinsics_test.cc"],
    deps = [
        ":intrinsics",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "std_tuple",
    hdrs = ["std_tuple.h"],
    deps = [
        ":intrinsics",
    ],
)

cc_library(
    name = "pair",
    hdrs = ["pair.h"],
    deps = [
        ":intrinsics",
    ],
)

cc_test(
    name = "pair_test",
    size = "small",
    srcs = ["pair_test.cc"],
    deps = [
        ":intrinsics",
        ":pair",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "array",
    hdrs = ["array.h"],
    deps = [
        ":intrinsics",
    ],
)

cc_test(
    name = "array_test",
    size = "small",
    srcs = ["array_test.cc"],
    deps = [
        ":array",
        ":intrinsics",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "internal_iterate",
    textual_hdrs = ["internal_iterate.h"],
    visibility = ["//visibility:private"],
    deps = [
        ":array",
        ":intrinsics",
        ":pair",
        ":std_tuple",
    ],
)

cc_library(
    name = "iterate",
    hdrs = ["iterate.h"],
    deps = [
        ":ignore_index",
        ":internal_iterate",
    ],
)

cc_test(
    name = "iterate_test",
    size = "small",
    srcs = ["iterate_test.cc"],
    deps = [
        ":iterate",
        ":std_tuple",
        "//visionai/util/tuple:test_util",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "for_each",
    hdrs = ["for_each.h"],
    deps = [
        ":array",
        ":ignore_index",
        ":intrinsics",
        ":iterate",
        ":pair",
        ":std_tuple",
    ],
)

cc_test(
    name = "for_each_test",
    size = "small",
    srcs = ["for_each_test.cc"],
    deps = [
        ":for_each",
        ":std_tuple",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)
