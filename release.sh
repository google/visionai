#!/bin/bash
# Copyright (c) 2023 Google LLC All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.

set -e

echo "startup --host_jvm_args=$JAVA_TOOL_OPTIONS" >> .bazelrc

# build debian package
debuild -b -us -uc

# build the pip wheel
bazel build -c opt //visionai/python/pip_package:build_pip_package

./bazel-bin/visionai/python/pip_package/build_pip_package

# move all artifacts to the folder
mkdir artifacts

mv ../visionai_*_amd64.deb artifacts/
mv visionai-*-py3-none-any.whl artifacts/