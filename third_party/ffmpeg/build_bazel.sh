#!/bin/bash

set -e

# When running inside a Docker sandbox, Bazel records stdout of this script on
# failure but not stderr, which omits error messages and can be confusing. To
# get around this, redirect all of stderr to stdout.
exec 2>&1

readonly BUILD_SOURCE="$1"
readonly BUILD_TARGET="$2"
readonly BAZELDIR=`pwd`
#readonly YASMEXE="${BAZELDIR}/$3"
readonly BUILD_DIR="$(mktemp -d)"
trap "rm -rf '${BUILD_DIR}'" EXIT

cp -r -L "$(pwd)/${BUILD_SOURCE}"/* "${BUILD_DIR}"

# Delete GPL licenced files
function delete_gpl_licenced_files() {
  gpl_licenced_file_list=(
      "${BUILD_DIR}/libavcodec/x86/flac_dsp_gpl.asm"
      "${BUILD_DIR}/libavcodec/x86/idct_mmx.c"
      "${BUILD_DIR}/libavfilter/x86/vf_removegrain.asm"
      "${BUILD_DIR}/compat/solaris/make_sunver.pl"
      "${BUILD_DIR}/doc/t2h.pm"
      "${BUILD_DIR}/doc/texi2pod.pl"
      "${BUILD_DIR}/libswresample/swresample-test.c"
      "${BUILD_DIR}/tests/tiny_ssim.c"
      "${BUILD_DIR}/libavfilter/vf_blackframe.c"
      "${BUILD_DIR}/libavfilter/vf_boxblur.c"
      "${BUILD_DIR}/libavfilter/vf_colormatrix.c"
      "${BUILD_DIR}/libavfilter/vf_cover_rect.c"
      "${BUILD_DIR}/libavfilter/vf_cropdetect.c"
      "${BUILD_DIR}/libavfilter/vf_delogo.c"
      "${BUILD_DIR}/libavfilter/vf_eq.c"
      "${BUILD_DIR}/libavfilter/vf_find_rect.c"
      "${BUILD_DIR}/libavfilter/vf_fspp.c"
      "${BUILD_DIR}/libavfilter/vf_geq.c"
      "${BUILD_DIR}/libavfilter/vf_histeq.c"
      "${BUILD_DIR}/libavfilter/vf_hqdn3d.c"
      "${BUILD_DIR}/libavfilter/vf_interlace.c"
      "${BUILD_DIR}/libavfilter/vf_kerndeint.c"
      "${BUILD_DIR}/libavfilter/vf_mcdeint.c"
      "${BUILD_DIR}/libavfilter/vf_mpdecimate.c"
      "${BUILD_DIR}/libavfilter/vf_owdenoise.c"
      "${BUILD_DIR}/libavfilter/vf_perspective.c"
      "${BUILD_DIR}/libavfilter/vf_phase.c"
      "${BUILD_DIR}/libavfilter/vf_pp.c"
      "${BUILD_DIR}/libavfilter/vf_pp7.c"
      "${BUILD_DIR}/libavfilter/vf_pullup.c"
      "${BUILD_DIR}/libavfilter/vf_repeatfields.c"
      "${BUILD_DIR}/libavfilter/vf_sab.c"
      "${BUILD_DIR}/libavfilter/vf_smartblur.c"
      "${BUILD_DIR}/libavfilter/vf_spp.c"
      "${BUILD_DIR}/libavfilter/vf_stereo3d.c"
      "${BUILD_DIR}/libavfilter/vf_super2xsai.c"
      "${BUILD_DIR}/libavfilter/vf_tinterlace.c"
      "${BUILD_DIR}/libavfilter/vf_uspp.c"
      "${BUILD_DIR}/libavfilter/vsrc_mptestsrc.c"
    )

  for FILE in ${gpl_licenced_file_list[@]}; do
    if [ -f ${FILE} ]; then
     rm -f ${FILE}
    fi
  done
}

delete_gpl_licenced_files

pushd "${BUILD_DIR}" &> /dev/null

./configure \
  --disable-runtime-cpudetect \
  --enable-shared \
  --disable-static \
  --disable-debug \
  --disable-programs \
  --disable-doc   \
  --disable-libxcb \
  --disable-nvenc \
  --disable-everything \
  --enable-decoder=h264 \
  --enable-demuxer=h264 \
  --enable-parser=h264 \
  --enable-decoder=hevc \
  --enable-demuxer=hevc \
  --enable-parser=hevc \
  --enable-decoder=mpegvideo \
  --enable-muxer=mpegvideo \
  --enable-encoder=rawvideo \
  --enable-decoder=rawvideo \
  --enable-muxer=rawvideo \
  --enable-demuxer=rawvideo \
  --enable-protocol=file \
  --enable-demuxer=image2 \
  --enable-muxer=image2 \
  --enable-encoder=png \
  --enable-parser=png \
  --enable-filter=scale \
  --enable-ffmpeg \
  --disable-gpl \
  --cc=${CC} \
  --host-cc=${CC} \
  --extra-cflags=${CFLAGS} \
  --extra-cxxflags=${CXXFLAGS} \
  --extra-ldflags=${LDFLAGS} \
  --nm=${NM} \
  --ar=${AR} \
  --strip=${STRIP} \
  --enable-pic \
  &>/dev/null || cat config.log

make -j$(nproc) &>/dev/null

popd &> /dev/null

readonly TARGET_DIR=$(dirname "${BUILD_TARGET}")
cp "${BUILD_DIR}/ffmpeg" "${TARGET_DIR}"
cp "${BUILD_DIR}/libavcodec/libavcodec.so.58" "${TARGET_DIR}"
cp "${BUILD_DIR}/libavdevice/libavdevice.so.58" "${TARGET_DIR}"
cp "${BUILD_DIR}/libavfilter/libavfilter.so.7" "${TARGET_DIR}"
cp "${BUILD_DIR}/libavformat/libavformat.so.58" "${TARGET_DIR}"
cp "${BUILD_DIR}/libavutil/libavutil.so.56" "${TARGET_DIR}"
cp "${BUILD_DIR}/libswresample/libswresample.so.3" "${TARGET_DIR}"
cp "${BUILD_DIR}/libswscale/libswscale.so.5" "${TARGET_DIR}"
cp "${BUILD_DIR}/libavutil/avconfig.h" "${TARGET_DIR}/libavutil"
