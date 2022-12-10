package(default_visibility = ["//visibility:public"])

licenses(["notice"])

cc_library(
    name = "sync_queue",
    hdrs = ["sync_queue.h"],
    deps = [
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_test(
    name = "sync_queue_test",
    srcs = ["sync_queue_test.cc"],
    deps = [
        ":sync_queue",
        "@com_google_googletest//:gtest_main",
    ],
)
