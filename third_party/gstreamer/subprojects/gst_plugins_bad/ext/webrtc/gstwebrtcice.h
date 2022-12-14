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

#ifndef __GST_WEBRTC_ICE_H__
#define __GST_WEBRTC_ICE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/sdp/sdp.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/webrtc/webrtc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/webrtc/fwd.h"

G_BEGIN_DECLS

#define GST_WEBRTC_ICE_ERROR gst_webrtc_ice_error_quark ()
GQuark gst_webrtc_ice_error_quark (void);

GType gst_webrtc_ice_get_type(void);
#define GST_TYPE_WEBRTC_ICE            (gst_webrtc_ice_get_type())
#define GST_WEBRTC_ICE(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_WEBRTC_ICE,GstWebRTCICE))
#define GST_IS_WEBRTC_ICE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_WEBRTC_ICE))
#define GST_WEBRTC_ICE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass) ,GST_TYPE_WEBRTC_ICE,GstWebRTCICEClass))
#define GST_IS_WEBRTC_ICE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass) ,GST_TYPE_WEBRTC_ICE))
#define GST_WEBRTC_ICE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj) ,GST_TYPE_WEBRTC_ICE,GstWebRTCICEClass))

struct _GstWebRTCICE
{
  GstObject                         parent;

  GstWebRTCICEGatheringState        ice_gathering_state;
  GstWebRTCICEConnectionState       ice_connection_state;

  GstUri                           *stun_server;
  GstUri                           *turn_server;

  GHashTable                       *turn_servers;

  GstWebRTCICEPrivate              *priv;

  guint                             min_rtp_port;
  guint                             max_rtp_port;
};

struct _GstWebRTCICEClass
{
  GstObjectClass            parent_class;
};

GstWebRTCICE *              gst_webrtc_ice_new                      (const gchar * name);
GstWebRTCICEStream *        gst_webrtc_ice_add_stream               (GstWebRTCICE * ice,
                                                                     guint session_id);
GstWebRTCICETransport *     gst_webrtc_ice_find_transport           (GstWebRTCICE * ice,
                                                                     GstWebRTCICEStream * stream,
                                                                     GstWebRTCICEComponent component);

gboolean                    gst_webrtc_ice_gather_candidates        (GstWebRTCICE * ice,
                                                                     GstWebRTCICEStream * stream);
/* FIXME: GstStructure-ize the candidate */
void                        gst_webrtc_ice_add_candidate            (GstWebRTCICE * ice,
                                                                     GstWebRTCICEStream * stream,
                                                                     const gchar * candidate);
gboolean                    gst_webrtc_ice_set_local_credentials    (GstWebRTCICE * ice,
                                                                     GstWebRTCICEStream * stream,
                                                                     gchar * ufrag,
                                                                     gchar * pwd);
gboolean                    gst_webrtc_ice_set_remote_credentials   (GstWebRTCICE * ice,
                                                                     GstWebRTCICEStream * stream,
                                                                     gchar * ufrag,
                                                                     gchar * pwd);
gboolean                    gst_webrtc_ice_add_turn_server          (GstWebRTCICE * ice,
                                                                     const gchar * uri);

void                        gst_webrtc_ice_set_is_controller        (GstWebRTCICE * ice,
                                                                     gboolean controller);
gboolean                    gst_webrtc_ice_get_is_controller        (GstWebRTCICE * ice);
void                        gst_webrtc_ice_set_force_relay          (GstWebRTCICE * ice,
                                                                     gboolean force_relay);
void                        gst_webrtc_ice_set_stun_server          (GstWebRTCICE * ice,
                                                                     const gchar * uri);
gchar *                     gst_webrtc_ice_get_stun_server          (GstWebRTCICE * ice);
void                        gst_webrtc_ice_set_turn_server          (GstWebRTCICE * ice,
                                                                     const gchar * uri);
gchar *                     gst_webrtc_ice_get_turn_server          (GstWebRTCICE * ice);

typedef void (*GstWebRTCIceOnCandidateFunc) (GstWebRTCICE * ice, guint stream_id, gchar * candidate, gpointer user_data);

void                        gst_webrtc_ice_set_on_ice_candidate     (GstWebRTCICE * ice,
                                                                     GstWebRTCIceOnCandidateFunc func,
                                                                     gpointer user_data,
                                                                     GDestroyNotify notify);

void                        gst_webrtc_ice_set_tos                  (GstWebRTCICE * ice,
                                                                     GstWebRTCICEStream * stream,
                                                                     guint tos);
G_END_DECLS

#endif /* __GST_WEBRTC_ICE_H__ */
