# Copyright 2003, Google Inc.  All rights reserved.
# -*- mode: python; -*-

# LIBRARIES

# No default_hdrs_check yet
package(default_visibility = ["//visibility:public"])

cc_library(
    name = "array2d",
    hdrs = ["array2d.h"],
    deps = [
        ":array_common",
        "@com_github_google_glog//:glog",
    ],
)

cc_library(
    name = "array3d",
    hdrs = ["array3d.h"],
    deps = [
        ":array_common",
        "@com_github_google_glog//:glog",
    ],
)

cc_library(
    name = "array_common",
    hdrs = ["array_common.h"],
    deps = [
    ],
)
