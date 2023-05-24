#!/usr/bin/env bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

#
# Script that installs kafka manager.

set -e

CMAK_VERSION=3.0.0.5
CMAK_ARCHIVE=https://github.com/yahoo/CMAK/archive/${CMAK_VERSION}.tar.gz

export DEBIAN_FRONTEND=noninteractive
runDeps=''
buildDeps='curl ca-certificates unzip apt-transport-https gnupg2 lsb-release'
apt-get update && apt-get install -y $runDeps $buildDeps --no-install-recommends

curl -s https://deb.nodesource.com/gpgkey/nodesource.gpg.key | apt-key add -
echo "deb https://deb.nodesource.com/node_8.x stretch main" > \
  /etc/apt/sources.list.d/nodesource.list
apt-get update && apt install -y --no-install-recommends nodejs

mkdir -p /opt/cmak-src
curl -SLs "${CMAK_ARCHIVE}" | tar -xzf - --strip-components=1 -C /opt/cmak-src

cd /opt/cmak-src
./sbt clean dist

cd /opt
unzip cmak-src/target/universal/cmak-$CMAK_VERSION.zip
mv cmak-$CMAK_VERSION cmak

rm -rf /root/.sbt /root/.ivy2 /opt/cmak-src

apt-get purge -y --auto-remove $buildDeps nodejs
rm -rf /var/lib/apt/lists/*
rm -rf /var/log/dpkg.log
rm -rf /var/log/alternatives.log
rm -rf /var/log/apt