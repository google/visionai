/* GStreamer
 * Copyright (C) <2005> Thomas Vander Stichele <thomas at apestaart dot org>
 *
 * gst.c: Unit test for gst.c
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
#include "third_party/gstreamer/subprojects/gstreamer/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstcheck.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstversion.h"

GST_START_TEST (test_init)
{
  /* don't segfault with NULL, NULL */
  gst_init (NULL, NULL);
  /* allow calling twice. well, actually, thrice. */
  gst_init (NULL, NULL);
}

GST_END_TEST;

GST_START_TEST (test_deinit)
{
  gst_init (NULL, NULL);

  /* gst_deinit will be called by test exit handler */
}

GST_END_TEST;

GST_START_TEST (test_deinit_sysclock)
{
  GstClock *clock;

  gst_init (NULL, NULL);

  clock = gst_system_clock_obtain ();
  gst_object_unref (clock);

  /* gst_deinit will be called by test exit handler */
}

GST_END_TEST;

/* tests if we can create an element from a compiled-in plugin */
GST_START_TEST (test_new_pipeline)
{
  GstElement *pipeline;

  pipeline = gst_pipeline_new ("pipeline");
  gst_object_unref (pipeline);
}

GST_END_TEST;

/* tests if we can load an element from a plugin */
GST_START_TEST (test_new_fakesrc)
{
  GstElement *element;

  element = gst_element_factory_make ("fakesrc", NULL);
  gst_object_unref (element);
}

GST_END_TEST;

GST_START_TEST (test_version)
{
  guint major, minor, micro, nano;
  gchar *version;

  gst_version (&major, &minor, &micro, &nano);
  assert_equals_int (major, GST_VERSION_MAJOR);

  version = gst_version_string ();
  fail_if (version == NULL);
  g_free (version);
}

GST_END_TEST;

static Suite *
gst_suite (void)
{
  Suite *s = suite_create ("Gst");
  TCase *tc_chain = tcase_create ("gst tests");
  const char *ck_fork = g_getenv ("CK_FORK");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_init);
  tcase_add_test (tc_chain, test_new_pipeline);
  tcase_add_test (tc_chain, test_new_fakesrc);
  tcase_add_test (tc_chain, test_version);
  /* run these last so the others don't fail if CK_FORK=no is being used.
   * only run single test for deinitialization if CK_FORK=no, so system exit
   * will make the single deinit call */
  tcase_add_test (tc_chain, test_deinit_sysclock);
  if (!ck_fork || (strcmp (ck_fork, "no") != 0))
    tcase_add_test (tc_chain, test_deinit);

  return s;
}

GST_CHECK_MAIN (gst);
