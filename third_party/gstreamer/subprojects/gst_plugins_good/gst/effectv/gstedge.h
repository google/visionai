/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2009> Sebastian Dröge <sebastian.droege@collabora.co.uk>
 *
 * EffecTV:
 * Copyright (C) 2001-2002 FUKUCHI Kentarou
 *
 * EdgeTV - detects edge and display it in good old computer way
 *
 * EffecTV is free software. This library is free software;
 * you can redistribute it and/or
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

#ifndef __GST_EDGE_H__
#define __GST_EDGE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideofilter.h"

G_BEGIN_DECLS

#define GST_TYPE_EDGETV \
  (gst_edgetv_get_type())
#define GST_EDGETV(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_EDGETV,GstEdgeTV))
#define GST_EDGETV_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_EDGETV,GstEdgeTVClass))
#define GST_IS_EDGETV(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_EDGETV))
#define GST_IS_EDGETV_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_EDGETV))

typedef struct _GstEdgeTV GstEdgeTV;
typedef struct _GstEdgeTVClass GstEdgeTVClass;

struct _GstEdgeTV
{
  GstVideoFilter videofilter;

  /* < private > */
  gint map_width, map_height;
  guint32 *map;
  gint video_width_margin;
};

struct _GstEdgeTVClass
{
  GstVideoFilterClass parent_class;
};

GType gst_edgetv_get_type (void);

G_END_DECLS

#endif /* __GST_EDGE_H__ */
