/*
 * GStreamer
 * Copyright (C) 2015 Matthew Waters <matthew@centricular.com>
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

#ifndef __GST_GTK_BASE_SINK_H__
#define __GST_GTK_BASE_SINK_H__

#include <gtk/gtk.h>
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideosink.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"

#include "third_party/gstreamer/subprojects/gst_plugins_good/ext/gtk/gtkgstbasewidget.h"

#define GST_TYPE_GTK_BASE_SINK            (gst_gtk_base_sink_get_type())
#define GST_GTK_BASE_SINK(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_GTK_BASE_SINK,GstGtkBaseSink))
#define GST_GTK_BASE_SINK_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_GTK_BASE_SINK,GstGtkBaseSinkClass))
#define GST_GTK_BASE_SINK_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_GTK_BASE_SINK, GstGtkBaseSinkClass))
#define GST_IS_GTK_BASE_SINK(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_GTK_BASE_SINK))
#define GST_IS_GTK_BASE_SINK_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_GTK_BASE_SINK))
#define GST_GTK_BASE_SINK_CAST(obj)       ((GstGtkBaseSink*)(obj))

G_BEGIN_DECLS

typedef struct _GstGtkBaseSink GstGtkBaseSink;
typedef struct _GstGtkBaseSinkClass GstGtkBaseSinkClass;

GType gst_gtk_base_sink_get_type (void);

/**
 * GstGtkBaseSink:
 *
 * Opaque #GstGtkBaseSink object
 */
struct _GstGtkBaseSink
{
  /* <private> */
  GstVideoSink         parent;

  GstVideoInfo         v_info;

  GtkGstBaseWidget     *widget;

  /* properties */
  gboolean             force_aspect_ratio;
  GBinding             *bind_aspect_ratio;

  gint                  par_n;
  gint                  par_d;
  GBinding             *bind_pixel_aspect_ratio;

  gint                  video_par_n;
  gint                  video_par_d;
  GBinding             *bind_video_aspect_ratio;

  gboolean              ignore_alpha;
  GBinding             *bind_ignore_alpha;

  GtkWidget            *window;
  gulong               widget_destroy_id;
  gulong               window_destroy_id;
};

/**
 * GstGtkBaseSinkClass:
 *
 * The #GstGtkBaseSinkClass struct only contains private data
 */
struct _GstGtkBaseSinkClass
{
  GstVideoSinkClass object_class;

  /* metadata */
  const gchar *window_title;

  /* virtuals */
  GtkWidget* (*create_widget) (void);
};

G_DEFINE_AUTOPTR_CLEANUP_FUNC (GstGtkBaseSink, gst_object_unref)

GtkWidget *
gst_gtk_base_sink_acquire_widget (GstGtkBaseSink * gtk_sink);

G_END_DECLS

#endif /* __GST_GTK_BASE_SINK_H__ */
