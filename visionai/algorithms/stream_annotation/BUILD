# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = [
        "//visionai:__subpackages__",
    ],
    licenses = ["notice"],  # Apache 2.0
)

cc_library(
    name = "geometry_lib",
    srcs = ["geometry_lib.cc"],
    hdrs = ["geometry_lib.h"],
    deps = [
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_test(
    name = "geometry_lib_test",
    srcs = ["geometry_lib_test.cc"],
    deps = [
        ":geometry_lib",
        "//visionai/testing/status:status_matchers",
        "@com_google_absl//absl/status",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "stream_annotation_util",
    srcs = ["stream_annotation_util.cc"],
    hdrs = ["stream_annotation_util.h"],
    deps = [
        ":geometry_lib",
        "//visionai/types:motion_vector",
        "//visionai/util:line_crossing_util",
        "@com_github_google_glog//:glog",
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/status:statusor",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/strings:str_format",
    ],
)
