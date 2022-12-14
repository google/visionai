/* GStreamer
 * Copyright (C) <2011> Jon Nordby <jononor@gmail.com>
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

#ifndef __GST_CAIRO_OVERLAY_H__
#define __GST_CAIRO_OVERLAY_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"

#include <cairo.h>
#include <cairo-gobject.h>

G_BEGIN_DECLS

#define GST_TYPE_CAIRO_OVERLAY (gst_cairo_overlay_get_type())
G_DECLARE_FINAL_TYPE (GstCairoOverlay, gst_cairo_overlay,
    GST, CAIRO_OVERLAY, GstBaseTransform)

struct _GstCairoOverlay {
  GstBaseTransform parent;

  /* properties */
  gboolean draw_on_transparent_surface;

  /* state */
  GstVideoInfo info;
  gboolean attach_compo_to_buffer;
};

GST_ELEMENT_REGISTER_DECLARE (cairooverlay);

G_END_DECLS

#endif /* __GST_CAIRO_OVERLAY_H__ */
