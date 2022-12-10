/* GStreamer
 * Copyright (C) 2012 Wim Taymans <wim.taymans@gmail.com>
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

#include "third_party/gstreamer/subprojects/gstreamer/config.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstcheck.h"

#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstadapter.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbaseparse.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbasesink.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbasesrc.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbasetransform.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbitreader.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbytereader.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbytewriter.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstcollectpads.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstpushsrc.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gsttypefindhelper.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/controller/gstinterpolationcontrolsource.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/controller/gstlfocontrolsource.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/controller/gsttriggercontrolsource.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/controller/gstargbcontrolbinding.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/controller/gstdirectcontrolbinding.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/controller/gsttimedvaluecontrolsource.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/net/gstnet.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/net/gstnetclientclock.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/net/gstnettimepacket.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/net/gstnettimeprovider.h"

#ifdef HAVE_CPU_I386
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_i386.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef __powerpc64__
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_ppc64.h"
#define HAVE_ABI_SIZES FALSE
#else
#ifdef __powerpc__
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_ppc32.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef HAVE_CPU_X86_64
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_x86_64.h"
#define HAVE_ABI_SIZES TRUE
#else
#ifdef HAVE_CPU_HPPA
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_hppa.h"
#define HAVE_ABI_SIZES FALSE
#else
#ifdef HAVE_CPU_SPARC
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_sparc.h"
#define HAVE_ABI_SIZES FALSE
#else
#ifdef HAVE_CPU_ARM
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_arm.h"
#define HAVE_ABI_SIZES TRUE
#else
/* in case someone wants to generate a new arch */
#include "third_party/gstreamer/subprojects/gstreamer/tests/check/libs/struct_i386.h"
#define HAVE_ABI_SIZES FALSE
#endif
#endif
#endif
#endif
#endif
#endif
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
