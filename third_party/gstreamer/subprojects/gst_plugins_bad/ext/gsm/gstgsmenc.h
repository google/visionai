/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
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

#ifndef __GST_GSMENC_H__
#define __GST_GSMENC_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/gstaudioencoder.h"

#ifdef GSM_HEADER_IN_SUBDIR
#include <gsm/gsm.h>
#else
#include <gsm.h>
#endif

G_BEGIN_DECLS

#define GST_TYPE_GSMENC \
  (gst_gsmenc_get_type())
#define GST_GSMENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GSMENC,GstGSMEnc))
#define GST_GSMENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GSMENC,GstGSMEncClass))
#define GST_IS_GSMENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GSMENC))
#define GST_IS_GSMENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GSMENC))

typedef struct _GstGSMEnc GstGSMEnc;
typedef struct _GstGSMEncClass GstGSMEncClass;

struct _GstGSMEnc
{
  GstAudioEncoder element;

  gsm state;
};

struct _GstGSMEncClass
{
  GstAudioEncoderClass parent_class;
};

GType gst_gsmenc_get_type (void);

GST_ELEMENT_REGISTER_DECLARE (gsmenc);

G_END_DECLS

#endif /* __GST_GSMENC_H__ */
