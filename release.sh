#!/bin/bash
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