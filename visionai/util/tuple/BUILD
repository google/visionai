package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],
)

cc_library(
    name = "for_each",
    hdrs = ["for_each.h"],
    deps = [
        "//visionai/util/tuple/components:for_each",
    ],
)

cc_library(
    name = "test_util",
    hdrs = ["test_util.h"],
    visibility = ["//visionai/util/tuple:__subpackages__"],
    deps = ["@com_google_googletest//:gtest_main"],
)
