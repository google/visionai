/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2012> Collabora Ltd.
 *   Author: Sebastian Dröge <sebastian.droege@collabora.co.uk>
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

#ifdef HAVE_CONFIG_H
#include "third_party/gstreamer/subprojects/gst_libav/config.h"
#endif

#include <assert.h>
#include <string.h>

#include "third_party/ffmpeg/libavcodec/avcodec.h"
#include "third_party/ffmpeg/libavutil/channel_layout.h"
#include "third_party/ffmpeg/libavutil/motion_vector.h"
#include "third_party/gstreamer/subprojects/gst_libav/ext/libav/gst-motion-meta.h"
#include "third_party/gstreamer/subprojects/gst_libav/ext/libav/gstav.h"

GType gst_motion_meta_api_get_type(void) {
  static GType type;
  static const gchar *tags[] = {NULL};

  if (g_once_init_enter(&type)) {
    GType _type = gst_meta_api_type_register("GstMotionMetaAPI", tags);
    GST_INFO("GstMotionMetaAPI registering.");
    g_print("GstMotionMetaAPI register.\n");
    g_once_init_leave(&type, _type);
  }

  return type;
}

static gboolean gst_motion_meta_transform(GstBuffer *transbuf, GstMeta *meta,
                                          GstBuffer *buffer, GQuark type,
                                          gpointer data) {
  GstMotionMeta *gst_motion_meta = (GstMotionMeta *)meta;
  GstMotionMeta *added_meta = gst_buffer_add_motion_meta(
      transbuf, gst_motion_meta->mvs, gst_motion_meta->size);
  if (!added_meta) return FALSE;
  return TRUE;
}

static gboolean gst_motion_meta_init(GstMeta *meta, gpointer params,
                                     GstBuffer *buffer) {
  GstMotionMeta *gst_motion_meta = (GstMotionMeta *)meta;

  gst_motion_meta->size = 0;
  gst_motion_meta->mvs = NULL;

  return TRUE;
}

static void gst_motion_meta_free(GstMeta *meta, GstBuffer *buffer) {
  GstMotionMeta *gst_motion_meta = (GstMotionMeta *)meta;
  g_free(gst_motion_meta->mvs);
  gst_motion_meta->mvs = NULL;
}

const GstMetaInfo *gst_motion_meta_get_info(void) {
  static const GstMetaInfo *meta_info = NULL;
  if (g_once_init_enter(&meta_info)) {
    const GstMetaInfo *mi = gst_meta_register(
        GST_MOTION_META_API_TYPE, "GstMotionMeta", sizeof(GstMotionMeta),
        gst_motion_meta_init, gst_motion_meta_free, gst_motion_meta_transform);
    g_once_init_leave(&meta_info, mi);
  }
  return meta_info;
}

/**
 * gst_buffer_add_motion_meta:
 * @buffer: a #GstBuffer
 * @mvs: (array length=size): The movion vectors associated with a single
 * AVFrame.
 * @size: The size in bytes of @mvs
 *
 * Attaches #GstMotionMeta metadata to @buffer with the given
 * parameters.
 *
 * Returns: (transfer none): the #GstMotionMeta on @buffer.
 */
GstMotionMeta *gst_buffer_add_motion_meta(GstBuffer *buffer,
                                          const AVMotionVector *mvs,
                                          gsize size) {
  g_return_val_if_fail(GST_IS_BUFFER(buffer), NULL);
  GstMotionMeta *gst_motion_meta =
      (GstMotionMeta *)gst_buffer_add_meta(buffer, GST_MOTION_META_INFO, NULL);
  g_return_val_if_fail(gst_motion_meta != NULL, NULL);
  if (mvs != NULL && size > 0) {
    gst_motion_meta->mvs = g_memdup2(mvs, size);
    gst_motion_meta->size = size;
  } else {
    gst_motion_meta->mvs = NULL;
    gst_motion_meta->size = 0;
  }

  return gst_motion_meta;
}
