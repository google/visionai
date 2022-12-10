/* GStreamer
 * Copyright (C) 2005 Wim Taymans <wim@fluendo.com>
 *               2011 Stefan Kost <ensonic@users.sf.net>
 *
 * libsabi.c: Unit test for ABI compatibility
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstcheck.h"

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/app/gstappsrc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/app/gstappsink.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/audio.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/gstaudiocdsrc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/gstaudioclock.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/gstaudiofilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/gstaudiosrc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/gstaudiosink.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/gstaudioringbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/streamvolume.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/fft/gstfft.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/fft/gstffts16.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/fft/gstffts32.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/fft/gstfftf32.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/fft/gstfftf64.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/pbutils/pbutils.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/riff/riff-media.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/riff/riff-read.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbaseaudiopayload.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbasedepayload.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbasepayload.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtcpbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtppayloads.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/gstrtsp.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/gstrtspconnection.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/gstrtspextension.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/gstrtspmessage.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/gstrtsprange.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/gstrtsptransport.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtsp/gstrtspurl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/sdp/gstsdp.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/sdp/gstsdpmessage.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/tag/tag.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/tag/gsttagdemux.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideofilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideosink.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/colorbalance.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/videodirection.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/videoorientation.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/videooverlay.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/navigation.h"
#if defined (TEST_GST_GL_ABI_CHECK)
#include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/gstgl-public-headers.h"
#endif

/* initial version of the file was generated using:
 * grep -A1 "<STRUCT>" ../../docs/libs/gst-plugins-base-libs-decl.txt | \
 * grep "<NAME>" | grep -v "Private" | sort | \
 * sed -e 's/<NAME>\(.*\)<\/NAME>/\  {\"\1\", sizeof (\1), 0\},/'
 *
 * it needs a bit of editing to remove opaque structs
 */

#ifdef HAVE_CPU_I386
# ifdef __APPLE__
#   include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_i386_osx.h"
#   define HAVE_ABI_SIZES FALSE
# else
#   include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_i386.h"
#   define HAVE_ABI_SIZES TRUE
# endif
#elif defined HAVE_CPU_X86_64
# include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_x86_64.h"
# define HAVE_ABI_SIZES TRUE
#elif defined HAVE_CPU_ARM
# include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_arm.h"
# define HAVE_ABI_SIZES FALSE
#elif defined HAVE_CPU_AARCH64
# include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_aarch64.h"
# define HAVE_ABI_SIZES FALSE
#elif defined HAVE_CPU_PPC
# include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_ppc32.h"
# define HAVE_ABI_SIZES TRUE
#elif defined HAVE_CPU_PPC64
# include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_ppc64.h"
# define HAVE_ABI_SIZES TRUE
#else /* in case someone wants to generate a new arch */
# include "third_party/gstreamer/subprojects/gst_plugins_base/tests/check/libs/struct_i386.h"
# define HAVE_ABI_SIZES FALSE
#endif

GST_START_TEST (test_ABI)
{
  gst_check_abi_list (list, HAVE_ABI_SIZES);
}

GST_END_TEST;

static Suite *
libsabi_suite (void)
{
  Suite *s = suite_create ("LibsABI");
  TCase *tc_chain = tcase_create ("size check");

  tcase_set_timeout (tc_chain, 0);

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_ABI);
  return s;
}

GST_CHECK_MAIN (libsabi);
