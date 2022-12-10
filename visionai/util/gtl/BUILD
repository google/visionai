package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

cc_library(
    name = "circularbuffer",
    hdrs = [
        "circularbuffer.h",
    ],
    deps = [
        ":container_logging",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/algorithm:container",
    ],
)

cc_test(
    name = "circularbuffer_test",
    srcs = ["circularbuffer_test.cc"],
    deps = [
        ":circularbuffer",
        "@com_google_absl//absl/strings",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "container_logging",
    hdrs = [
        "container_logging.h",
    ],
)

cc_test(
    name = "container_logging_test",
    srcs = ["container_logging_test.cc"],
    deps = [
        ":container_logging",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:span",
        "@com_google_googletest//:gtest_main",
    ],
)
