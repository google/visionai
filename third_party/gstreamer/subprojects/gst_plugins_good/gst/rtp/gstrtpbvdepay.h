/* GStreamer
 * Copyright (C) <2009> Wim Taymans <wim.taymans@gmail.com>
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

#ifndef __GST_RTP_BV_DEPAY_H__
#define __GST_RTP_BV_DEPAY_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbasedepayload.h"

G_BEGIN_DECLS

typedef struct _GstRTPBVDepay GstRTPBVDepay;
typedef struct _GstRTPBVDepayClass GstRTPBVDepayClass;

#define GST_TYPE_RTP_BV_DEPAY \
  (gst_rtp_bv_depay_get_type())
#define GST_RTP_BV_DEPAY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_RTP_BV_DEPAY,GstRTPBVDepay))
#define GST_RTP_BV_DEPAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RTP_BV_DEPAY,GstRTPBVDepayClass))
#define GST_IS_RTP_BV_DEPAY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RTP_BV_DEPAY))
#define GST_IS_RTP_BV_DEPAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RTP_BV_DEPAY))

struct _GstRTPBVDepay
{
  GstRTPBaseDepayload depayload;

  gint mode;
};

struct _GstRTPBVDepayClass
{
  GstRTPBaseDepayloadClass parent_class;
};

GType gst_rtp_bv_depay_get_type (void);

G_END_DECLS

#endif /* __GST_RTP_BV_DEPAY_H__ */
