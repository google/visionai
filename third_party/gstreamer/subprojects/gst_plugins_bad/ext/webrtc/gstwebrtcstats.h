/* GStreamer
 * Copyright (C) 2017 Matthew Waters <matthew@centricular.com>
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

#ifndef __GST_WEBRTC_STATS_H__
#define __GST_WEBRTC_STATS_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/sdp/sdp.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/webrtc/webrtc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/webrtc/fwd.h"

G_BEGIN_DECLS

G_GNUC_INTERNAL
GstStructure *     gst_webrtc_bin_create_stats         (GstWebRTCBin * webrtc,
                                                        GstPad * pad);

G_END_DECLS

#endif /* __GST_WEBRTC_STATS_H__ */
