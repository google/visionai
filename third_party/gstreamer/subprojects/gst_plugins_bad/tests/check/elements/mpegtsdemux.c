/* GStreamer
 *
 * Copyright (C) 2020 LTN Global Communications
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

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstcheck.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstharness.h"

#define PACKETSIZE 188

/* Output of the following pipeline, split into standard 188-bytes packets:
 *
 * audiotestsrc num-buffers=1 samplesperbuffer=1280 wave=silence
 * ! avenc_aac ! mpegtsmux
 */
static const guint8 aac_ts[] = {
  0x47, 0x40, 0x00, 0x31, 0xa6, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0x00, 0x00, 0xb0, 0x0d, 0x00, 0x01, 0xc1, 0x00, 0x00,
  0x00, 0x01, 0xe0, 0x20, 0xa2, 0xc3, 0x29, 0x41,

  0x47, 0x40, 0x20, 0x31, 0x9b, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x00, 0x02, 0xb0, 0x18, 0x00, 0x01, 0xc1, 0x00,
  0x00, 0xe0, 0x41, 0xf0, 0x00, 0x0f, 0xe0, 0x41, 0xf0, 0x06, 0x0a, 0x04,
  0x65, 0x6e, 0x00, 0x00, 0xfa, 0xa6, 0x03, 0x09,

  0x47, 0x40, 0x41, 0x31, 0x8d, 0x10, 0x09, 0xa7, 0xd6, 0x87, 0x7e, 0x00,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x24, 0x81, 0x80, 0x05, 0x21,
  0x4d, 0x3f, 0xb2, 0x01, 0xff, 0xf1, 0x50, 0x40, 0x03, 0x9f, 0xfc, 0xde,
  0x02, 0x00, 0x4c, 0x61, 0x76, 0x63, 0x35, 0x38, 0x2e, 0x35, 0x34, 0x2e,
  0x31, 0x30, 0x30, 0x00, 0x02, 0x30, 0x40, 0x0e,

  0x47, 0x40, 0x41, 0x32, 0x9e, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x01, 0xc0, 0x00,
  0x13, 0x81, 0x80, 0x05, 0x21, 0x4d, 0x3f, 0xc2, 0x53, 0xff, 0xf1, 0x50,
  0x40, 0x01, 0x7f, 0xfc, 0x01, 0x18, 0x20, 0x07,

  0x47, 0x40, 0x41, 0x33, 0x9e, 0x10, 0x09, 0xa7, 0xde, 0xb0, 0xfe, 0x00,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x01, 0xc0, 0x00,
  0x13, 0x81, 0x80, 0x05, 0x21, 0x4d, 0x3f, 0xd2, 0xa7, 0xff, 0xf1, 0x50,
  0x40, 0x01, 0x7f, 0xfc, 0x01, 0x18, 0x20, 0x07
};

G_STATIC_ASSERT (sizeof aac_ts % PACKETSIZE == 0);
static const guint aac_ts_packets = sizeof aac_ts / PACKETSIZE;

/* Buffers output by tsdemux for above data */
static const guint8 aac_data[] = {
  0xff, 0xf1, 0x50, 0x40, 0x03, 0x9f, 0xfc, 0xde, 0x02, 0x00, 0x4c, 0x61,
  0x76, 0x63, 0x35, 0x38, 0x2e, 0x35, 0x34, 0x2e, 0x31, 0x30, 0x30, 0x00,
  0x02, 0x30, 0x40, 0x0e,

  0xff, 0xf1, 0x50, 0x40, 0x01, 0x7f, 0xfc, 0x01, 0x18, 0x20, 0x07,

  0xff, 0xf1, 0x50, 0x40, 0x01, 0x7f, 0xfc, 0x01, 0x18, 0x20, 0x07
};

/* Padding packet */
static const guint8 padding_ts[] = {
  0x47, 0x1f, 0xff, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

G_STATIC_ASSERT (sizeof padding_ts == PACKETSIZE);

GST_START_TEST (test_tsparse_simple)
{
  GstHarness *h = gst_harness_new ("tsparse");
  GstBuffer *buf;

  gst_harness_set_src_caps_str (h, "video/mpegts,systemstream=true");
  gst_harness_set_sink_caps_str (h,
      "video/mpegts,systemstream=true,packetsize=" G_STRINGIFY (PACKETSIZE));

  buf =
      gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY, (guint8 *) aac_ts,
      sizeof aac_ts, 0, sizeof aac_ts, NULL, NULL);
  buf = gst_harness_push_and_pull (h, buf);
  gst_check_buffer_data (buf, aac_ts, sizeof aac_ts);
  gst_buffer_unref (buf);

  gst_harness_push_event (h, gst_event_new_eos ());
  fail_unless (gst_harness_buffers_in_queue (h) == 0);

  gst_harness_teardown (h);
}

GST_END_TEST;

GST_START_TEST (test_tsparse_align_auto)
{
  GstHarness *h = gst_harness_new ("tsparse");
  GstBuffer *buf;
  gsize i;

  gst_harness_set_src_caps_str (h, "video/mpegts,systemstream=true");
  gst_harness_set_sink_caps_str (h,
      "video/mpegts,systemstream=true,packetsize=" G_STRINGIFY (PACKETSIZE));

  buf =
      gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY, (guint8 *) aac_ts,
      sizeof aac_ts, 0, sizeof aac_ts, NULL, NULL);
  for (i = 0; i < sizeof aac_ts; i += 1) {
    fail_unless (gst_harness_push (h, gst_buffer_copy_region (buf,
                GST_BUFFER_COPY_MEMORY, i, 1)) == GST_FLOW_OK);
  }
  gst_buffer_unref (buf);

  /* FIXME: Using automatic alignment, feeding tsparse one byte at a time should
   * make it return each packet in a separate buffer (5 buffers total) but at
   * the time of writing it only outputs 2 buffers.
   */

  gst_harness_push_event (h, gst_event_new_eos ());
  fail_unless (gst_harness_buffers_in_queue (h) == aac_ts_packets,
      "Expected %u buffers, got %u", aac_ts_packets,
      gst_harness_buffers_in_queue (h));

  for (i = 0; i < sizeof aac_ts; i += PACKETSIZE) {
    buf = gst_harness_pull (h);
    gst_check_buffer_data (buf, aac_ts + i, PACKETSIZE);
    gst_buffer_unref (buf);
  }

  gst_harness_teardown (h);
}

GST_END_TEST;

GST_START_TEST (test_tsparse_align_fuse)
{
  GstHarness *h = gst_harness_new ("tsparse");
  GstBuffer *buf;
  gsize i;

  gst_harness_set (h, "tsparse", "alignment", aac_ts_packets, NULL);

  gst_harness_set_src_caps_str (h, "video/mpegts,systemstream=true");
  gst_harness_set_sink_caps_str (h,
      "video/mpegts,systemstream=true,packetsize=" G_STRINGIFY (PACKETSIZE));

  buf =
      gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY, (guint8 *) aac_ts,
      sizeof aac_ts, 0, sizeof aac_ts, NULL, NULL);
  for (i = 0; i < sizeof aac_ts; i += 1) {
    fail_unless (gst_harness_push (h, gst_buffer_copy_region (buf,
                GST_BUFFER_COPY_MEMORY, i, 1)) == GST_FLOW_OK);
  }
  gst_buffer_unref (buf);

  gst_harness_push_event (h, gst_event_new_eos ());
  fail_unless (gst_harness_buffers_in_queue (h) == 1,
      "Expected 1 buffer, got %u", gst_harness_buffers_in_queue (h));

  buf = gst_harness_pull (h);
  gst_check_buffer_data (buf, aac_ts, sizeof aac_ts);
  gst_buffer_unref (buf);

  gst_harness_teardown (h);
}

GST_END_TEST;


GST_START_TEST (test_tsparse_align_split)
{
  GstHarness *h = gst_harness_new ("tsparse");
  GstBuffer *buf;
  gsize i;

  gst_harness_set (h, "tsparse", "alignment", 1, NULL);

  gst_harness_set_src_caps_str (h, "video/mpegts,systemstream=true");
  gst_harness_set_sink_caps_str (h,
      "video/mpegts,systemstream=true,packetsize=" G_STRINGIFY (PACKETSIZE));

  buf =
      gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY, (guint8 *) aac_ts,
      sizeof aac_ts, 0, sizeof aac_ts, NULL, NULL);
  fail_unless (gst_harness_push (h, buf) == GST_FLOW_OK);

  gst_harness_push_event (h, gst_event_new_eos ());
  fail_unless (gst_harness_buffers_in_queue (h) == aac_ts_packets,
      "Expected %u buffers, got %u", aac_ts_packets,
      gst_harness_buffers_in_queue (h));

  for (i = 0; i < sizeof aac_ts; i += PACKETSIZE) {
    buf = gst_harness_pull (h);
    gst_check_buffer_data (buf, aac_ts + i, PACKETSIZE);
    gst_buffer_unref (buf);
  }

  gst_harness_teardown (h);
}

GST_END_TEST;

GST_START_TEST (test_tsparse_padding)
{
  GstHarness *h = gst_harness_new ("tsparse");
  GstBuffer *buf, *padding;

  gst_harness_set_src_caps_str (h, "video/mpegts,systemstream=true");
  gst_harness_set_sink_caps_str (h,
      "video/mpegts,systemstream=true,packetsize=" G_STRINGIFY (PACKETSIZE));

  buf =
      gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY, (guint8 *) aac_ts,
      sizeof aac_ts, 0, sizeof aac_ts, NULL, NULL);
  padding =
      gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY,
      (guint8 *) padding_ts, sizeof padding_ts, 0, sizeof padding_ts, NULL,
      NULL);

  fail_unless (gst_harness_push (h, gst_buffer_ref (buf)) == GST_FLOW_OK);
  fail_unless (gst_harness_push (h, gst_buffer_ref (padding)) == GST_FLOW_OK);
  fail_unless (gst_harness_push (h, buf) == GST_FLOW_OK);
  fail_unless (gst_harness_push (h, gst_buffer_ref (padding)) == GST_FLOW_OK);
  fail_unless (gst_harness_push (h, padding) == GST_FLOW_OK);

  buf = gst_harness_pull (h);
  gst_check_buffer_data (buf, aac_ts, sizeof aac_ts);
  gst_buffer_unref (buf);

  padding = gst_harness_pull (h);
  gst_check_buffer_data (padding, padding_ts, sizeof padding_ts);
  gst_buffer_unref (padding);

  buf = gst_harness_pull (h);
  gst_check_buffer_data (buf, aac_ts, sizeof aac_ts);
  gst_buffer_unref (buf);

  padding = gst_harness_pull (h);
  gst_check_buffer_data (padding, padding_ts, sizeof padding_ts);
  gst_buffer_unref (padding);

  padding = gst_harness_pull (h);
  gst_check_buffer_data (padding, padding_ts, sizeof padding_ts);
  gst_buffer_unref (padding);

  gst_harness_push_event (h, gst_event_new_eos ());
  fail_unless (gst_harness_buffers_in_queue (h) == 0);

  gst_harness_teardown (h);
}

GST_END_TEST;

static void
tsdemux_simple_pad_added (GstElement * tsdemux, GstPad * pad, GstHarness * h)
{
  fail_unless (g_strcmp0 (GST_PAD_NAME (pad), "audio_0_0041") == 0);
  gst_harness_add_element_src_pad (h, pad);
}

GST_START_TEST (test_tsdemux_simple)
{
  GstHarness *h = gst_harness_new_with_padnames ("tsdemux", "sink", NULL);
  GstBuffer *buf;
  GstCaps *caps;
  GstSegment segment;

  caps = gst_caps_from_string ("video/mpegts,systemstream=true");
  gst_harness_push_event (h, gst_event_new_caps (caps));
  gst_caps_unref (caps);

  gst_segment_init (&segment, GST_FORMAT_BYTES);
  gst_harness_push_event (h, gst_event_new_segment (&segment));

  gst_harness_set_sink_caps_str (h,
      "audio/mpeg,mpegversion=4,stream-format=adts");

  g_signal_connect (h->element, "pad-added",
      G_CALLBACK (tsdemux_simple_pad_added), h);

  buf =
      gst_buffer_new_wrapped_full (GST_MEMORY_FLAG_READONLY, (guint8 *) aac_ts,
      sizeof aac_ts, 0, sizeof aac_ts, NULL, NULL);
  fail_unless (gst_harness_push (h, buf) == GST_FLOW_OK);
  gst_harness_push_event (h, gst_event_new_eos ());

  buf = gst_harness_take_all_data_as_buffer (h);
  gst_check_buffer_data (buf, aac_data, sizeof aac_data);
  gst_buffer_unref (buf);

  gst_harness_teardown (h);
}

GST_END_TEST;

static Suite *
mpegtsdemux_suite (void)
{
  Suite *s = suite_create ("mpegtsdemux");
  TCase *tc;

  tc = tcase_create ("tsparse");
  suite_add_tcase (s, tc);
  tcase_add_test (tc, test_tsparse_simple);
  tcase_skip_broken_test (tc, test_tsparse_align_auto);
  tcase_add_test (tc, test_tsparse_align_fuse);
  tcase_add_test (tc, test_tsparse_align_split);
  tcase_add_test (tc, test_tsparse_padding);

  tc = tcase_create ("tsdemux");
  suite_add_tcase (s, tc);
  tcase_add_test (tc, test_tsdemux_simple);

  return s;
}

GST_CHECK_MAIN (mpegtsdemux);
