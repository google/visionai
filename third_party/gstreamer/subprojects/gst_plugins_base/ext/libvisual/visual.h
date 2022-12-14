/* GStreamer
 * Copyright (C) 2004 Benjamin Otte <otte@gnome.org>
 *               2012 Stefan Sauer <ensonic@users.sf.net>
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

#ifndef __GST_VISUAL_H__
#define __GST_VISUAL_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstadapter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideopool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/audio.h"
#include <libvisual/libvisual.h>

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/pbutils/gstaudiovisualizer.h"

G_BEGIN_DECLS

#define GST_TYPE_VISUAL (gst_visual_get_type())
#define GST_IS_VISUAL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_VISUAL))
#define GST_VISUAL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_VISUAL,GstVisual))
#define GST_IS_VISUAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_VISUAL))
#define GST_VISUAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_VISUAL,GstVisualClass))
#define GST_VISUAL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_VISUAL, GstVisualClass))

typedef struct _GstVisual GstVisual;
typedef struct _GstVisualClass GstVisualClass;

struct _GstVisual
{
  GstAudioVisualizer element;

  /* libvisual stuff */
  VisAudio *audio;
  VisVideo *video;
  VisActor *actor;
};

struct _GstVisualClass
{
  GstAudioVisualizerClass parent_class;

  VisPluginRef *plugin;
};

void gst_visual_class_init (gpointer g_class, gpointer class_data);

GType gst_visual_get_type (void);

G_END_DECLS

#endif /* __GST_VISUAL_H__ */

