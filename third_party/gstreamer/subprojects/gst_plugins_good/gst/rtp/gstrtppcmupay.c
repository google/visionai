/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2005> Edgard Lima <edgard.lima@gmail.com>
 * Copyright (C) <2005> Nokia Corporation <kai.vehmanen@nokia.com>
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
#  include "third_party/gstreamer/subprojects/gst_plugins_good/config.h"
#endif

#include <stdlib.h>
#include <string.h>
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbuffer.h"

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtp/gstrtpelements.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtp/gstrtppcmupay.h"

static GstStaticPadTemplate gst_rtp_pcmu_pay_sink_template =
GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("audio/x-mulaw, channels=(int)1, rate=(int)8000")
    );

static GstStaticPadTemplate gst_rtp_pcmu_pay_src_template =
    GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-rtp, "
        "media = (string) \"audio\", "
        "payload = (int) " GST_RTP_PAYLOAD_PCMU_STRING ", "
        "clock-rate = (int) 8000, " "encoding-name = (string) \"PCMU\"; "
        "application/x-rtp, "
        "media = (string) \"audio\", "
        "payload = (int) " GST_RTP_PAYLOAD_DYNAMIC_STRING ", "
        "clock-rate = (int) [1, MAX ], " "encoding-name = (string) \"PCMU\"")
    );

static gboolean gst_rtp_pcmu_pay_setcaps (GstRTPBasePayload * payload,
    GstCaps * caps);

#define gst_rtp_pcmu_pay_parent_class parent_class
G_DEFINE_TYPE (GstRtpPcmuPay, gst_rtp_pcmu_pay,
    GST_TYPE_RTP_BASE_AUDIO_PAYLOAD);
GST_ELEMENT_REGISTER_DEFINE_WITH_CODE (rtppcmupay, "rtppcmupay",
    GST_RANK_SECONDARY, GST_TYPE_RTP_PCMU_PAY, rtp_element_init (plugin));

static void
gst_rtp_pcmu_pay_class_init (GstRtpPcmuPayClass * klass)
{
  GstElementClass *gstelement_class;
  GstRTPBasePayloadClass *gstrtpbasepayload_class;

  gstelement_class = (GstElementClass *) klass;
  gstrtpbasepayload_class = (GstRTPBasePayloadClass *) klass;

  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_rtp_pcmu_pay_sink_template);
  gst_element_class_add_static_pad_template (gstelement_class,
      &gst_rtp_pcmu_pay_src_template);

  gst_element_class_set_static_metadata (gstelement_class, "RTP PCMU payloader",
      "Codec/Payloader/Network/RTP",
      "Payload-encodes PCMU audio into a RTP packet",
      "Edgard Lima <edgard.lima@gmail.com>");

  gstrtpbasepayload_class->set_caps = gst_rtp_pcmu_pay_setcaps;
}

static void
gst_rtp_pcmu_pay_init (GstRtpPcmuPay * rtppcmupay)
{
  GstRTPBaseAudioPayload *rtpbaseaudiopayload;

  rtpbaseaudiopayload = GST_RTP_BASE_AUDIO_PAYLOAD (rtppcmupay);

  GST_RTP_BASE_PAYLOAD (rtppcmupay)->pt = GST_RTP_PAYLOAD_PCMU;
  GST_RTP_BASE_PAYLOAD (rtppcmupay)->clock_rate = 8000;

  /* tell rtpbaseaudiopayload that this is a sample based codec */
  gst_rtp_base_audio_payload_set_sample_based (rtpbaseaudiopayload);

  /* octet-per-sample is 1 for PCM */
  gst_rtp_base_audio_payload_set_sample_options (rtpbaseaudiopayload, 1);
}

static gboolean
gst_rtp_pcmu_pay_setcaps (GstRTPBasePayload * payload, GstCaps * caps)
{
  gboolean res;

  gst_rtp_base_payload_set_options (payload, "audio",
      payload->pt != GST_RTP_PAYLOAD_PCMU, "PCMU", 8000);
  res = gst_rtp_base_payload_set_outcaps (payload, NULL);

  return res;
}
