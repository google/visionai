#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

#
# Script that installs kafka in a container.

set -e
set -x
export DEBIAN_FRONTEND=noninteractive
export KAFKA_VERSION=2.8.0
export SCALA_VERSION=2.13

runDeps='ca-certificates netcat-openbsd'
buildDeps='curl gnupg dirmngr'

apt-get update && apt-get install -y $runDeps $buildDeps --no-install-recommends

curl -sLS -o KEYS https://www.apache.org/dist/kafka/KEYS
gpg --import KEYS && rm KEYS

SCALA_BINARY_VERSION=$(echo $SCALA_VERSION | cut -f 1-2 -d '.')

mkdir -p /opt/kafka
curl -sLS -o kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz.asc \
  https://archive.apache.org/dist/kafka/$KAFKA_VERSION/kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz.asc
curl -sLS -o kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz \
  "https://archive.apache.org/dist/kafka/$KAFKA_VERSION/kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz"
gpg --verify kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz.asc \
  kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz
tar xzf kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz --strip-components=1 -C /opt/kafka

rm kafka_$SCALA_BINARY_VERSION-$KAFKA_VERSION.tgz
rm -rf /opt/kafka/site-docs

apt-get purge -y --auto-remove $buildDeps
rm -rf /var/lib/apt/lists
rm -rf /var/log/dpkg.log /var/log/alternatives.log
rm -rf /var/log/apt
rm -rf /root/.gnupg
