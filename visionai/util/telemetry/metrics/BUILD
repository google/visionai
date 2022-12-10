package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "stats",
    srcs = ["stats.cc"],
    hdrs = ["stats.h"],
    deps = [
        ":constants",
        ":stats_macros",
        "@com_census_instrumentation_cpp//opencensus/stats",
        "@com_census_instrumentation_cpp//opencensus/tags",
        "@com_github_jupp0r_prometheus_cpp//core",  # buildcleaner: keep
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "metrics_recorder",
    srcs = [
        "metrics_recorder.cc",
    ],
    hdrs = [
        "metrics_recorder.h",
    ],
    deps = [
        "@com_census_instrumentation_cpp//opencensus/stats",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/base:core_headers",
        "@com_google_absl//absl/container:flat_hash_map",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "noop_metrics_recorder",
    hdrs = [
        "noop_metrics_recorder.h",
    ],
    deps = [
        ":metrics_recorder",
        "@com_census_instrumentation_cpp//opencensus/stats",
    ],
)

cc_library(
    name = "prometheus_metrics_recorder",
    srcs = [
        "prometheus_metrics_recorder.cc",
    ],
    hdrs = [
        "prometheus_metrics_recorder.h",
    ],
    deps = [
        ":metrics_recorder",
        ":stats",
        "@com_census_instrumentation_cpp//opencensus/exporters/stats/prometheus:prometheus_exporter",
        "@com_census_instrumentation_cpp//opencensus/stats",
        "@com_census_instrumentation_cpp//opencensus/tags",
        "@com_github_jupp0r_prometheus_cpp//core",  # buildcleaner: keep
        "@com_github_jupp0r_prometheus_cpp//pull",  # buildcleaner: keep
        "@com_google_absl//absl/time",
    ],
)

cc_library(
    name = "constants",
    hdrs = ["constants.h"],
)

cc_library(
    name = "stats_macros",
    hdrs = ["stats_macros.h"],
)

cc_library(
    name = "labels",
    hdrs = ["labels.h"],
    deps = ["//visionai/proto:cluster_selection_cc_proto"],
)
