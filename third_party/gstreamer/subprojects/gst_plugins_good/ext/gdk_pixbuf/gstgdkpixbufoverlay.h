/* GStreamer GdkPixbuf overlay
 * Copyright (C) 2012-2014 Tim-Philipp Müller <tim centricular net>
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

#ifndef _GST_GDK_PIXBUF_OVERLAY_H_
#define _GST_GDK_PIXBUF_OVERLAY_H_

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideofilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video-overlay-composition.h"

#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#define GST_TYPE_GDK_PIXBUF_OVERLAY (gst_gdk_pixbuf_overlay_get_type())
G_DECLARE_FINAL_TYPE (GstGdkPixbufOverlay, gst_gdk_pixbuf_overlay,
    GST, GDK_PIXBUF_OVERLAY, GstVideoFilter)

typedef enum {
  GST_GDK_PIXBUF_POSITIONING_PIXELS_RELATIVE_TO_EDGES,
  GST_GDK_PIXBUF_POSITIONING_PIXELS_ABSOLUTE
} GstGdkPixbufPositioningMode;

/**
 * GstGdkPixbufOverlay:
 *
 * The opaque element instance structure.
 */
struct _GstGdkPixbufOverlay
{
  GstVideoFilter               videofilter;

  /* properties */
  gchar                      * location;

  /* pixbuf set via pixbuf property */
  GdkPixbuf                  * pixbuf;

  gint                         offset_x;
  gint                         offset_y;

  gdouble                      relative_x;
  gdouble                      relative_y;

  gdouble                      coef_x;
  gdouble                      coef_y;

  GstGdkPixbufPositioningMode  positioning_mode;

  gint                         overlay_width;
  gint                         overlay_height;

  gdouble                      alpha;

  /* the loaded image, as BGRA/ARGB pixels, with GstVideoMeta */
  GstBuffer                  * pixels;               /* OBJECT_LOCK */

  GstVideoOverlayComposition * comp;

  /* render position or dimension has changed */
  gboolean                     update_composition;
};

G_END_DECLS

#endif
