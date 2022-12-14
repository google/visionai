/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 * Copyright (C) 2013 Juan Manuel Borges Caño <juanmabcmail@gmail.com>
 *
 * gstladspasink.h: Header for LADSPA sink
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

#ifndef __GST_LADSPA_SINK_H__
#define __GST_LADSPA_SINK_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbasesink.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/ladspa/gstladspautils.h"

G_BEGIN_DECLS

#define GST_TYPE_LADSPA_SINK              (gst_ladspa_sink_get_type())
#define GST_LADSPA_SINK(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_LADSPA_SINK,GstLADSPASink))
#define GST_LADSPA_SINK_CAST(obj)         ((GstLADSPASink *) (obj))
#define GST_LADSPA_SINK_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_LADSPA_SINK,GstLADSPASinkClass))
#define GST_LADSPA_SINK_CLASS_CAST(klass) ((GstLADSPASinkClass *) (klass))
#define GST_LADSPA_SINK_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_LADSPA_SINK,GstLADSPASinkClass))
#define GST_IS_LADSPA_SINK(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_LADSPA_SINK))
#define GST_IS_LADSPA_SINK_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_LADSPA_SINK))

typedef struct _GstLADSPASink GstLADSPASink;

typedef struct _GstLADSPASinkClass GstLADSPASinkClass;

struct _GstLADSPASink
{
  GstBaseSink parent;

  GstLADSPA ladspa;

  GstAudioInfo info;

  gint num_buffers;
  gint num_buffers_left;
};

struct _GstLADSPASinkClass
{
  GstBaseSinkClass parent_class;

  GstLADSPAClass ladspa;
};

GType
gst_ladspa_sink_get_type (void);

void
ladspa_register_sink_element (GstPlugin * plugin, GstStructure *ladspa_meta);

void
gst_my_base_sink_class_add_pad_template (GstBaseSinkClass * base_class,
    GstCaps * sinkcaps);

G_END_DECLS

#endif /* __GST_LADSPA_SINK_H__ */
