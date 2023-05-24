#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

#
# Installs python and pip packages.

set -e

apt-get install -y --no-install-recommends \
    python3-dev python3-pip python3-setuptools

pip3 --disable-pip-version-check install \
  absl-py \
  gcloud \
  google-api-python-client \
  google-auth \
  grpc-google-iam-v1 \
  grpcio \
  numpy \
  proto-plus \
  protobuf \
  requests \
  wheel
