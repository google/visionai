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


#ifndef __GST_RTP_MP2T_PAY_H__
#define __GST_RTP_MP2T_PAY_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/rtp/gstrtpbasepayload.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstadapter.h"

G_BEGIN_DECLS

typedef struct _GstRTPMP2TPay GstRTPMP2TPay;
typedef struct _GstRTPMP2TPayClass GstRTPMP2TPayClass;

#define GST_TYPE_RTP_MP2T_PAY \
  (gst_rtp_mp2t_pay_get_type())
#define GST_RTP_MP2T_PAY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_RTP_MP2T_PAY,GstRTPMP2TPay))
#define GST_RTP_MP2T_PAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RTP_MP2T_PAY,GstRTPMP2TPayClass))
#define GST_IS_RTP_MP2T_PAY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RTP_MP2T_PAY))
#define GST_IS_RTP_MP2T_PAY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RTP_MP2T_PAY))

struct _GstRTPMP2TPay
{
  GstRTPBasePayload payload;
  
  GstAdapter  *adapter;
  GstClockTime first_ts;
  GstClockTime duration;
};

struct _GstRTPMP2TPayClass
{
  GstRTPBasePayloadClass parent_class;
};

GType gst_rtp_mp2t_pay_get_type (void);

G_END_DECLS

#endif /* __GST_RTP_MP2T_PAY_H__ */
