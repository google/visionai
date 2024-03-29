# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

load("@rules_cc//cc:defs.bzl", "cc_proto_library")

package(
    default_visibility = ["//visibility:public"],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "util",
    srcs = [
        "util.cc",
    ],
    hdrs = [
        "util.h",
    ],
    deps = [
        "//visionai/types:motion_vector",
        "//visionai/util/array:array2d",
        "//visionai/util/array:array3d",
        "//visionai/util/gtl:circularbuffer",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "util_test",
    srcs = ["util_test.cc"],
    deps = [
        ":util",
        "//visionai/types:motion_vector",
        "//visionai/util/array:array2d",
        "//visionai/util/array:array3d",
        "//visionai/util/gtl:circularbuffer",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_googletest//:gtest_main",
    ],
)

proto_library(
    name = "motion_vector_based_motion_detector_config_proto",
    srcs = ["motion_vector_based_motion_detector_config.proto"],
)

cc_proto_library(
    name = "motion_vector_based_motion_detector_config_cc_proto",
    deps = [":motion_vector_based_motion_detector_config_proto"],
)

cc_library(
    name = "motion_vector_based_motion_detector",
    srcs = ["motion_vector_based_motion_detector.cc"],
    hdrs = ["motion_vector_based_motion_detector.h"],
    deps = [
        ":motion_vector_based_motion_detector_config_cc_proto",
        ":util",
        "//visionai/algorithms/stream_annotation:geometry_lib",
        "//visionai/algorithms/stream_annotation:stream_annotation_util",
        "//visionai/types:motion_vector",
        "//visionai/util/array:array3d",
        "//visionai/util/gtl:circularbuffer",
        "//visionai/util/status:status_macros",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
    ],
)
