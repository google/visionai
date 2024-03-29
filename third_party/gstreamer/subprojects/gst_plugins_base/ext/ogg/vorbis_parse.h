/* GStreamer
 * Copyright (C) 2004 Wim Taymans <wim@fluendo.com>
 *
 * gstoggdemux.c: ogg stream demuxer
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

#ifndef __GST_VORBIS_PARSE_H__
#define __GST_VORBIS_PARSE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_GNUC_INTERNAL
void gst_parse_vorbis_header_packet (GstOggStream * pad, ogg_packet * packet);
G_GNUC_INTERNAL
int gst_parse_vorbis_setup_packet (GstOggStream * pad, ogg_packet * op);

#endif /* __GST_VORBIS_PARSE_H__ */
