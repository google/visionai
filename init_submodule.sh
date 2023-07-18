#!/bin/bash
# Copyright 2022 Google LLC
#
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file or at
# https://developers.google.com/open-source/licenses/bsd

set -o xtrace
git submodule update --init --recursive
# Install commit-msg hook in each submodule.
git submodule foreach --recursive '
  git checkout master
  f=`git rev-parse --git-dir`/hooks/commit-msg
  mkdir -p $(dirname $f)
  curl -Lo $f https://gerrit-review.googlesource.com/tools/hooks/commit-msg
  chmod +x $f
'
# Enable git commmands to recurse into submodules by default.
git config fetch.recurseSubmodules true


# To update submodule to latest remote commit:
# git submodule update --remote --merge