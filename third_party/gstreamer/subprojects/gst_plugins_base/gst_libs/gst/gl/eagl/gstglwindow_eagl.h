/*
 * GStreamer
 * Copyright (C) 2014 Sebastian Dröge <sebastian@centricular.com>
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

#ifndef __GST_GL_WINDOW_EAGL_H__
#define __GST_GL_WINDOW_EAGL_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gl.h"

G_BEGIN_DECLS

#define GST_TYPE_GL_WINDOW_EAGL         (gst_gl_window_eagl_get_type())
#define GST_GL_WINDOW_EAGL(o)           (G_TYPE_CHECK_INSTANCE_CAST((o), GST_TYPE_GL_WINDOW_EAGL, GstGLWindowEagl))
#define GST_GL_WINDOW_EAGL_CLASS(k)     (G_TYPE_CHECK_CLASS((k), GST_TYPE_GL_WINDOW_EAGL, GstGLWindowEaglClass))
#define GST_IS_GL_WINDOW_EAGL(o)        (G_TYPE_CHECK_INSTANCE_TYPE((o), GST_TYPE_GL_WINDOW_EAGL))
#define GST_IS_GL_WINDOW_EAGL_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE((k), GST_TYPE_GL_WINDOW_EAGL))
#define GST_GL_WINDOW_EAGL_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS((o), GST_TYPE_GL_WINDOW_EAGL, GstGLWindowEaglClass))

typedef struct _GstGLWindowEagl        GstGLWindowEagl;
typedef struct _GstGLWindowEaglPrivate GstGLWindowEaglPrivate;
typedef struct _GstGLWindowEaglClass   GstGLWindowEaglClass;

struct _GstGLWindowEagl {
  /*< private >*/
  GstGLWindow parent;

  /*< private >*/
  GstGLWindowEaglPrivate *priv;
  
  gpointer _reserved[GST_PADDING];
};

struct _GstGLWindowEaglClass {
  /*< private >*/
  GstGLWindowClass parent_class;

  /*< private >*/
  gpointer _reserved[GST_PADDING_LARGE];
};

GType gst_gl_window_eagl_get_type     (void);

GstGLWindowEagl * gst_gl_window_eagl_new (GstGLDisplay * display);

G_END_DECLS

#endif /* __GST_GL_WINDOW_EAGL_H__ */
