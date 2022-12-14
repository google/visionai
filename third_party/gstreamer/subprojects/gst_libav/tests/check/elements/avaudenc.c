/* GStreamer
 *
 * Copyright (C) 2020 Seungha Yang <seungha@centricular.com>
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
#include "third_party/gstreamer/subprojects/gst_libav/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstcheck.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstharness.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/audio.h"

GST_START_TEST (test_audioenc_drain)
{
  GstHarness *h;
  GstAudioInfo info;
  GstBuffer *in_buf;
  gint i = 0;
  gint num_output = 0;
  GstFlowReturn ret;
  GstSegment segment;
  GstCaps *caps;
  gint samples_per_buffer = 1024;
  gint rate = 44100;
  gint size;
  GstClockTime duration;

  h = gst_harness_new ("avenc_aac");
  fail_unless (h != NULL);

  gst_audio_info_set_format (&info, GST_AUDIO_FORMAT_F32, rate, 1, NULL);

  caps = gst_audio_info_to_caps (&info);
  gst_harness_set_src_caps (h, gst_caps_copy (caps));

  duration = gst_util_uint64_scale_int (samples_per_buffer, GST_SECOND, rate);
  size = samples_per_buffer * GST_AUDIO_INFO_BPF (&info);

  for (i = 0; i < 2; i++) {
    in_buf = gst_buffer_new_and_alloc (size);

    gst_buffer_memset (in_buf, 0, 0, size);

    /* small rounding error would be expected, but should be fine */
    GST_BUFFER_PTS (in_buf) = i * duration;
    GST_BUFFER_DURATION (in_buf) = duration;

    ret = gst_harness_push (h, in_buf);

    fail_unless (ret == GST_FLOW_OK, "GstFlowReturn was %s",
        gst_flow_get_name (ret));
  }

  gst_segment_init (&segment, GST_FORMAT_TIME);
  fail_unless (gst_segment_set_running_time (&segment, GST_FORMAT_TIME,
          2 * duration));

  /* Push new eos event to drain encoder */
  fail_unless (gst_harness_push_event (h, gst_event_new_eos ()));

  /* And start new stream */
  fail_unless (gst_harness_push_event (h,
          gst_event_new_stream_start ("new-stream-id")));
  gst_harness_set_src_caps (h, caps);
  fail_unless (gst_harness_push_event (h, gst_event_new_segment (&segment)));

  in_buf = gst_buffer_new_and_alloc (size);

  GST_BUFFER_PTS (in_buf) = 2 * duration;
  GST_BUFFER_DURATION (in_buf) = duration;

  ret = gst_harness_push (h, in_buf);
  fail_unless (ret == GST_FLOW_OK, "GstFlowReturn was %s",
      gst_flow_get_name (ret));

  /* Finish encoding and drain again */
  fail_unless (gst_harness_push_event (h, gst_event_new_eos ()));
  do {
    GstBuffer *out_buf = NULL;

    out_buf = gst_harness_try_pull (h);
    if (out_buf) {
      num_output++;
      gst_buffer_unref (out_buf);
      continue;
    }

    break;
  } while (1);

  fail_unless (num_output >= 3);

  gst_harness_teardown (h);
}

GST_END_TEST;

static Suite *
avaudenc_suite (void)
{
  Suite *s = suite_create ("avaudenc");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_test (tc_chain, test_audioenc_drain);

  return s;
}

GST_CHECK_MAIN (avaudenc)
