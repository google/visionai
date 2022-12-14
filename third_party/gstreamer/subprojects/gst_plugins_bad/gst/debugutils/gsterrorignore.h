/* 
 * GStreamer
 * Copyright (C) 2015 Vivia Nikolaidou <vivia@toolsonair.com>
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
 
#ifndef __GST_ERROR_IGNORE_H__
#define __GST_ERROR_IGNORE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_BEGIN_DECLS

#define GST_TYPE_ERROR_IGNORE            (gst_error_ignore_get_type())
#define GST_ERROR_IGNORE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_ERROR_IGNORE,GstErrorIgnore))
#define GST_IS_ERROR_IGNORE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_ERROR_IGNORE))
#define GST_ERROR_IGNORE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass) ,GST_TYPE_ERROR_IGNORE,GstErrorIgnoreClass))
#define GST_IS_ERROR_IGNORE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass) ,GST_TYPE_ERROR_IGNORE))
#define GST_ERROR_IGNORE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj) ,GST_TYPE_ERROR_IGNORE,GstErrorIgnoreClass))

typedef struct _GstErrorIgnore      GstErrorIgnore;
typedef struct _GstErrorIgnoreClass GstErrorIgnoreClass;

struct _GstErrorIgnore {
  GstElement parent;

  GstPad *srcpad, *sinkpad;

  gboolean keep_pushing;
  gboolean ignore_error;
  gboolean ignore_notlinked;
  gboolean ignore_notnegotiated;
  gboolean ignore_eos;
  GstFlowReturn convert_to;
};

struct _GstErrorIgnoreClass {
  GstElementClass parent_class;
};

GType gst_error_ignore_get_type (void);

gboolean gst_error_ignore_plugin_init (GstPlugin * plugin);

G_END_DECLS

#endif /* __GST_ERROR_IGNORE_H__ */
