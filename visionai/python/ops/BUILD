# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_library(
    name = "ops",
    srcs = [
        "__init__.py",
    ],
    srcs_version = "PY3",
    deps = [
        ":gen_ops",
    ],
)

# TODO(b/279183970): Use a genrule calling pyopgen directly to generate this
# file. Check this in for now to start.
py_library(
    name = "gen_ops",
    srcs = [
        "gen_ops.py",
    ],
    srcs_version = "PY3",
    deps = [
        "//visionai/python/lva",
    ],
)
