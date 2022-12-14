/*
 * GStreamer
 * Copyright (C) 2009 David A. Schleef <ds@schleef.org>
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

#ifndef __GST_GTK_H__
#define __GST_GTK_H__

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/videooverlay.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

void gst_video_overlay_set_gtk_window (GstVideoOverlay *videooverlay, GtkWidget *window);
gboolean gst_gtk_handle_need_context (GstBus *bus, GstMessage *msg, gpointer data);

G_END_DECLS

#endif

