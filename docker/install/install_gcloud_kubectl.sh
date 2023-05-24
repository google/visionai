#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd


set -e

# Install gcloud and kubectl.
apt-get update && apt-get install -y --no-install-recommends \
    apt-transport-https \
    ca-certificates \
    gnupg

echo \
  "deb [signed-by=/usr/share/keyrings/cloud.google.gpg] https://packages.cloud.google.com/apt cloud-sdk main" \
  | tee -a /etc/apt/sources.list.d/google-cloud-sdk.list

curl https://packages.cloud.google.com/apt/doc/apt-key.gpg \
  | apt-key --keyring /usr/share/keyrings/cloud.google.gpg add -

apt-get update && apt-get install -y --no-install-recommends \
    google-cloud-sdk \
    kubectl

# Enable kubectl bash completion.
echo "source <(kubectl completion bash)" >> /etc/bash.bashrc
echo "alias k=kubectl" >> /etc/bash.bashrc
