#!/bin/bash
#
# Copyright 2008 Google Inc. All Rights Reserved.
# Author: mboerger@google.com (Marcus Borger)
#
# Script to update or locally build into googlify sandbox (http://go/googlify).
# pcre was not googlified; this script exists for dependent googlified libs.

libname=pcre
version=8.44
description='Perl-compatible regular expressions.'
mainurl='http://www.pcre.org/'
license='BSD'
lictype="notice"
dirname="${libname}"

googlify="${PWD}/../../devtools/googlify/googlify.sh"
if [[ ! -f "${googlify}" ]]; then
  googlify=/home/build/google3/devtools/googlify/googlify.sh
fi

test -z "${1}" && mode="--help" || mode="${1}"
shift

modes='
full:           --build --install --pkgconfig --package
build:          --build --install --pkgconfig
package:        --package
googlify-build: --googlify-build --googlifylog /dev/null
'

function func_do_config {
  unset AR
  ./configure \
    ${CONFIG_SETUP} \
    ${CONFIG_SHARED} \
    ${CONFIG_STATIC} \
    ${CONFIG_PIC} \
    ${CONFIG_HOST} \
    --enable-utf8 \
    --enable-unicode-properties
}

export -f func_do_config

${googlify} \
  --update "$0" "${modes}" ${mode} \
  --libname ${libname} \
  --version ${version} \
  --description "${description}" \
  --mainurl "${mainurl}" \
  --license "${license}" \
  --lictype "${lictype}" \
  --depends ${depends[*]} \
  --forcemainbuild \
  "$@"
