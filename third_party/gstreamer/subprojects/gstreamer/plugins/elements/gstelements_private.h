/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gst_private.h: Private header for within libgst
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

#ifndef __GST_ELEMENTS_PRIVATE_H__
#define __GST_ELEMENTS_PRIVATE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_BEGIN_DECLS

G_GNUC_INTERNAL
gchar *   gst_buffer_get_flags_string (GstBuffer *buffer);

G_GNUC_INTERNAL
gchar *   gst_buffer_get_meta_string (GstBuffer * buffer);

G_GNUC_INTERNAL
GstFlowReturn  gst_writev_buffer (GstObject * sink, gint fd, GstPoll * fdset,
                                  GstBuffer * buffer,
                                  guint64 * bytes_written, guint64 skip,
                                  gint max_transient_error_timeout, guint64 current_position,
                                  gboolean * flushing);

G_GNUC_INTERNAL
GstFlowReturn  gst_writev_buffer_list (GstObject * sink, gint fd, GstPoll * fdset,
                                       GstBufferList * buffer_list,
                                       guint64 * bytes_written, guint64 skip,
                                       gint max_transient_error_timeout, guint64 current_position,
                                       gboolean * flushing);

G_GNUC_INTERNAL
GstFlowReturn  gst_writev_mem         (GstObject * sink, gint fd, GstPoll * fdset,
                                       const guint8 *data, guint size,
                                       guint64 * bytes_written, guint64 skip,
                                       gint max_transient_error_timeout, guint64 current_position,
                                       gboolean * flushing);

G_END_DECLS

#endif /* __GST_ELEMENTS_PRIVATE_H__ */
