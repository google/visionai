#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

#
# Script that installs Go in a container.
#

set -e

GO_VERSION="1.16"

mkdir -p /go
cd /go

curl -fSsl -O https://dl.google.com/go/go$GO_VERSION.linux-amd64.tar.gz
tar -C /usr/local -xzf go$GO_VERSION.linux-amd64.tar.gz

rm -rf /go
