# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

package(
    default_visibility = ["//visionai:__subpackages__"],
    licenses = ["notice"],
)

py_binary(
    name = "build_pip_package",
    srcs = [
        "build_pip_package.py",
    ],
    data = [
        "MANIFEST.in",
        "requirements.txt",
        "setup.py",
        "//visionai:visionai_py",
    ],
    srcs_version = "PY3",
)
