/* GStreamer GdkPixbuf sink
 * Copyright (C) 2006-2008 Tim-Philipp Müller <tim centricular net>
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

#ifndef GST_GDK_PIXBUF_SINK_H
#define GST_GDK_PIXBUF_SINK_H

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideosink.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

#define GST_TYPE_GDK_PIXBUF_SINK (gst_gdk_pixbuf_sink_get_type())
G_DECLARE_FINAL_TYPE (GstGdkPixbufSink, gst_gdk_pixbuf_sink,
    GST, GDK_PIXBUF_SINK, GstVideoSink)

/**
 * GstGdkPixbufSink:
 *
 * Opaque element structure.
 */
struct _GstGdkPixbufSink
{
  GstVideoSink  basesink;

  /*< private >*/

  /* current caps */
  GstVideoInfo info;
  gint         width;
  gint         height;
  gint         par_n;
  gint         par_d;
  gboolean     has_alpha;

  /* properties */
  gboolean     post_messages;
  GdkPixbuf  * last_pixbuf;
};

#endif /* GST_GDK_PIXBUF_SINK_H */

