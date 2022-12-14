/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
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


#ifndef __GST_DVDEMUX_H__
#define __GST_DVDEMUX_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include <libdv/dv.h>
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstadapter.h"

G_BEGIN_DECLS

#define GST_TYPE_DVDEMUX (gst_dvdemux_get_type())
G_DECLARE_FINAL_TYPE (GstDVDemux, gst_dvdemux, GST, DVDEMUX, GstElement)

typedef gboolean (*GstDVDemuxSeekHandler) (GstDVDemux *demux, GstPad * pad, GstEvent * event);

struct _GstDVDemux {
  GstElement     element;

  GstPad        *sinkpad;
  GstPad        *videosrcpad;
  GstPad        *audiosrcpad;

  gboolean       have_group_id;
  guint          group_id;

  dv_decoder_t  *decoder;

  GstAdapter    *adapter;
  gint           frame_len;

  /* video params */
  gint           framerate_numerator;
  gint           framerate_denominator;
  gint           height;
  gboolean       wide;
  /* audio params */
  gint           frequency;
  gint           channels;

  gboolean       discont;
  gint64         frame_offset;
  gint64         audio_offset;
  gint64         video_offset;

  GstDVDemuxSeekHandler seek_handler;
  GstSegment     byte_segment;
  gboolean       upstream_time_segment;
  GstSegment     time_segment;
  gboolean       need_segment;
  guint32        segment_seqnum;
  gboolean       new_media;
  int            frames_since_new_media;
  
  gint           found_header; /* ATOMIC */
  GstEvent      *seek_event;
  GstEvent	*pending_segment;
  GstEvent      *tag_event;

  gint16        *audio_buffers[4];
};

G_END_DECLS

#endif /* __GST_DVDEMUX_H__ */
