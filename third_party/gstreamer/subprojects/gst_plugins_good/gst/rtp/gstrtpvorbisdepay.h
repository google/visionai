/* GStreamer
 * Copyright (C) <2006> Wim Taymans <wim.taymans@gmail.com>
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

#ifndef __GST_RTP_VORBIS_DEPAY_H__
#define __GST_RTP_VORBIS_DEPAY_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstadapter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbasedepayload.h"

G_BEGIN_DECLS

#define GST_TYPE_RTP_VORBIS_DEPAY \
  (gst_rtp_vorbis_depay_get_type())
#define GST_RTP_VORBIS_DEPAY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_RTP_VORBIS_DEPAY,GstRtpVorbisDepay))
#define GST_RTP_VORBIS_DEPAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RTP_VORBIS_DEPAY,GstRtpVorbisDepayClass))
#define GST_IS_RTP_VORBIS_DEPAY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RTP_VORBIS_DEPAY))
#define GST_IS_RTP_VORBIS_DEPAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RTP_VORBIS_DEPAY))

typedef struct _GstRtpVorbisDepay GstRtpVorbisDepay;
typedef struct _GstRtpVorbisDepayClass GstRtpVorbisDepayClass;

typedef struct _GstRtpVorbisConfig {
  guint32  ident;
  GList   *headers;
} GstRtpVorbisConfig;

struct _GstRtpVorbisDepay
{
  GstRTPBaseDepayload parent;

  GList              *configs;
  GstRtpVorbisConfig *config;

  GstAdapter         *adapter;
  gboolean            assembling;
};

struct _GstRtpVorbisDepayClass
{
  GstRTPBaseDepayloadClass parent_class;
};

GType gst_rtp_vorbis_depay_get_type (void);

G_END_DECLS

#endif /* __GST_RTP_VORBIS_DEPAY_H__ */
