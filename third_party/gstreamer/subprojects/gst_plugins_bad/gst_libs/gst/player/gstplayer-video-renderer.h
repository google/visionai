/* GStreamer
 *
 * Copyright (C) 2014-2015 Sebastian Dröge <sebastian@centricular.com>
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

#ifndef __GST_PLAYER_VIDEO_RENDERER_H__
#define __GST_PLAYER_VIDEO_RENDERER_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/player/gstplayer-types.h"

G_BEGIN_DECLS

typedef struct _GstPlayerVideoRenderer GstPlayerVideoRenderer;
typedef struct _GstPlayerVideoRendererInterface GstPlayerVideoRendererInterface;

#define GST_TYPE_PLAYER_VIDEO_RENDERER                (gst_player_video_renderer_get_type ())
#define GST_PLAYER_VIDEO_RENDERER(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_PLAYER_VIDEO_RENDERER, GstPlayerVideoRenderer))
#define GST_IS_PLAYER_VIDEO_RENDERER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_PLAYER_VIDEO_RENDERER))
#define GST_PLAYER_VIDEO_RENDERER_GET_INTERFACE(inst) (G_TYPE_INSTANCE_GET_INTERFACE ((inst), GST_TYPE_PLAYER_VIDEO_RENDERER, GstPlayerVideoRendererInterface))

struct _GstPlayerVideoRendererInterface {
  GTypeInterface parent_iface;

  GstElement * (*create_video_sink) (GstPlayerVideoRenderer * self, GstPlayer * player);
};

GST_PLAYER_API
GType        gst_player_video_renderer_get_type       (void);

G_END_DECLS

#endif /* __GST_PLAYER_VIDEO_RENDERER_H__ */
