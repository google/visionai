#! /bin/sh
#
# Wrapper around test scripts from PCRE library to make them runnable under
# google3.
#

set -e

SRC_DIR="${0%/*}"


cd ${SRC_DIR}
cp -a RunTest RunGrepTest pcretest pcregrep testdata/ ${TEST_TMPDIR}

cd ${TEST_TMPDIR}
./RunTest "$@" && ./RunGrepTest

