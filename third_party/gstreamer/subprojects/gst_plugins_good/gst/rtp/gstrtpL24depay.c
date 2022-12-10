/* GStreamer
 * Copyright (C) <2007> Wim Taymans <wim.taymans@gmail.com>
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

/**
 * SECTION:element-rtpL24depay
 * @title: rtpL24depay
 * @see_also: rtpL24pay
 *
 * Extract raw audio from RTP packets according to RFC 3190, section 4.
 * For detailed information see: http://www.rfc-editor.org/rfc/rfc3190.txt
 *
 * ## Example pipeline
 * |[
 * gst-launch-1.0 udpsrc caps='application/x-rtp, media=(string)audio, clock-rate=(int)44100, encoding-name=(string)L24, encoding-params=(string)1, channels=(int)1, payload=(int)96' ! rtpL24depay ! pulsesink
 * ]| This example pipeline will depayload an RTP raw audio stream. Refer to
 * the rtpL24pay example to create the RTP stream.
 *
 */

#ifdef HAVE_CONFIG_H
#include "third_party/gstreamer/subprojects/gst_plugins_good/config.h"
#endif

#include <string.h>
#include <stdlib.h>

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/audio.h"

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtp/gstrtpelements.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtp/gstrtpL24depay.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtp/gstrtpchannels.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtp/gstrtputils.h"

GST_DEBUG_CATEGORY_STATIC (rtpL24depay_debug);
#define GST_CAT_DEFAULT (rtpL24depay_debug)

static GstStaticPadTemplate gst_rtp_L24_depay_src_template =
GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-raw, "
        "format = (string) S24BE, "
        "layout = (string) interleaved, "
        "rate = (int) [ 1, MAX ], " "channels = (int) [ 1, MAX ]")
    );

static GstStaticPadTemplate gst_rtp_L24_depay_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-rtp, "
        "media = (string) \"audio\", " "clock-rate = (int) [ 1, MAX ], "
        "encoding-name = (string) \"L24\"")
    );

#define gst_rtp_L24_depay_parent_class parent_class
G_DEFINE_TYPE (GstRtpL24Depay, gst_rtp_L24_depay, GST_TYPE_RTP_BASE_DEPAYLOAD);
GST_ELEMENT_REGISTER_DEFINE_WITH_CODE (rtpL24depay, "rtpL24depay",
    GST_RANK_SECONDARY, GST_TYPE_RTP_L24_DEPAY, rtp_element_init (plugin));

static gboolean gst_rtp_L24_depay_setcaps (GstRTPBaseDepayload * depayload,
    GstCaps * caps);
static GstBuffer *gst_rtp_L24_depay_process (GstRTPBaseDepayload * depayload,
    GstRTPBuffer * rtp);

static void
gst_rtp_L24_depay_class_init (GstRtpL24DepayClass * klass)
{
  GstElementClass *gstelement_class;
  GstRTPBaseDepayloadClass *gstrtpbasedepayload_class;

  gstelement_class = (GstElementClass *) klass;
  gstrtpbasedepayload_class = (GstRTPBaseDepayloadClass *) klass;

  gstrtpbasedepayload_class->set_caps = gst_rtp_L24_depay_setcaps;
  gstrtpbasedepayload_class->process_rtp_packet = gst_rtp_L24_depay_process;

  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_rtp_L24_depay_src_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_rtp_L24_depay_sink_template);

  gst_element_class_set_static_metadata (gstelement_class,
      "RTP audio depayloader", "Codec/Depayloader/Network/RTP",
      "Extracts raw 24-bit audio from RTP packets",
      "Zeeshan Ali <zak147@yahoo.com>," "Wim Taymans <wim.taymans@gmail.com>,"
      "David Holroyd <dave@badgers-in-foil.co.uk>");

  GST_DEBUG_CATEGORY_INIT (rtpL24depay_debug, "rtpL24depay", 0,
      "Raw Audio RTP Depayloader");
}

static void
gst_rtp_L24_depay_init (GstRtpL24Depay * rtpL24depay)
{
}

static gint
gst_rtp_L24_depay_parse_int (GstStructure * structure, const gchar * field,
    gint def)
{
  const gchar *str;
  gint res;

  if ((str = gst_structure_get_string (structure, field)))
    return atoi (str);

  if (gst_structure_get_int (structure, field, &res))
    return res;

  return def;
}

static gboolean
gst_rtp_L24_depay_setcaps (GstRTPBaseDepayload * depayload, GstCaps * caps)
{
  GstStructure *structure;
  GstRtpL24Depay *rtpL24depay;
  gint clock_rate, payload;
  gint channels;
  GstCaps *srccaps;
  gboolean res;
  const gchar *channel_order;
  const GstRTPChannelOrder *order;
  GstAudioInfo *info;

  rtpL24depay = GST_RTP_L24_DEPAY (depayload);

  structure = gst_caps_get_structure (caps, 0);

  payload = 96;
  gst_structure_get_int (structure, "payload", &payload);
  /* no fixed mapping, we need clock-rate */
  channels = 0;
  clock_rate = 0;

  /* caps can overwrite defaults */
  clock_rate =
      gst_rtp_L24_depay_parse_int (structure, "clock-rate", clock_rate);
  if (clock_rate == 0)
    goto no_clockrate;

  channels =
      gst_rtp_L24_depay_parse_int (structure, "encoding-params", channels);
  if (channels == 0) {
    channels = gst_rtp_L24_depay_parse_int (structure, "channels", channels);
    if (channels == 0) {
      /* channels defaults to 1 otherwise */
      channels = 1;
    }
  }

  depayload->clock_rate = clock_rate;

  info = &rtpL24depay->info;
  gst_audio_info_init (info);
  info->finfo = gst_audio_format_get_info (GST_AUDIO_FORMAT_S24BE);
  info->rate = clock_rate;
  info->channels = channels;
  info->bpf = (info->finfo->width / 8) * channels;

  /* add channel positions */
  channel_order = gst_structure_get_string (structure, "channel-order");

  order = gst_rtp_channels_get_by_order (channels, channel_order);
  rtpL24depay->order = order;
  if (order) {
    memcpy (info->position, order->pos,
        sizeof (GstAudioChannelPosition) * channels);
    gst_audio_channel_positions_to_valid_order (info->position, info->channels);
  } else {
    GST_ELEMENT_WARNING (rtpL24depay, STREAM, DECODE,
        (NULL), ("Unknown channel order '%s' for %d channels",
            GST_STR_NULL (channel_order), channels));
    /* create default NONE layout */
    gst_rtp_channels_create_default (channels, info->position);
    info->flags |= GST_AUDIO_FLAG_UNPOSITIONED;
  }

  srccaps = gst_audio_info_to_caps (info);
  res = gst_pad_set_caps (depayload->srcpad, srccaps);
  gst_caps_unref (srccaps);

  return res;

  /* ERRORS */
no_clockrate:
  {
    GST_ERROR_OBJECT (depayload, "no clock-rate specified");
    return FALSE;
  }
}

static GstBuffer *
gst_rtp_L24_depay_process (GstRTPBaseDepayload * depayload, GstRTPBuffer * rtp)
{
  GstRtpL24Depay *rtpL24depay;
  GstBuffer *outbuf;
  gint payload_len;
  gboolean marker;

  rtpL24depay = GST_RTP_L24_DEPAY (depayload);

  payload_len = gst_rtp_buffer_get_payload_len (rtp);

  if (payload_len <= 0)
    goto empty_packet;

  GST_DEBUG_OBJECT (rtpL24depay, "got payload of %d bytes", payload_len);

  outbuf = gst_rtp_buffer_get_payload_buffer (rtp);
  marker = gst_rtp_buffer_get_marker (rtp);

  if (marker) {
    /* mark talk spurt with RESYNC */
    GST_BUFFER_FLAG_SET (outbuf, GST_BUFFER_FLAG_RESYNC);
  }

  outbuf = gst_buffer_make_writable (outbuf);
  if (outbuf) {
    gst_rtp_drop_non_audio_meta (rtpL24depay, outbuf);
  }
  if (rtpL24depay->order &&
      !gst_audio_buffer_reorder_channels (outbuf,
          rtpL24depay->info.finfo->format, rtpL24depay->info.channels,
          rtpL24depay->info.position, rtpL24depay->order->pos)) {
    goto reorder_failed;
  }

  return outbuf;

  /* ERRORS */
empty_packet:
  {
    GST_ELEMENT_WARNING (rtpL24depay, STREAM, DECODE,
        ("Empty Payload."), (NULL));
    return NULL;
  }
reorder_failed:
  {
    GST_ELEMENT_ERROR (rtpL24depay, STREAM, DECODE,
        ("Channel reordering failed."), (NULL));
    return NULL;
  }
}
