/* GStreamer
 *
 * Copyright (C) 2019 Collabora Ltd.
 *   Author: Stéphane Cerveau <scerveau@collabora.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library (COPYING); if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __GSTMPDSUBREPRESENTATIONNODE_H__
#define __GSTMPDSUBREPRESENTATIONNODE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/dash/gstmpdhelper.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/dash/gstmpdrepresentationbasenode.h"

G_BEGIN_DECLS

#define GST_TYPE_MPD_SUB_REPRESENTATION_NODE gst_mpd_sub_representation_node_get_type ()
G_DECLARE_FINAL_TYPE (GstMPDSubRepresentationNode, gst_mpd_sub_representation_node, GST, MPD_SUB_REPRESENTATION_NODE, GstMPDRepresentationBaseNode)

struct _GstMPDSubRepresentationNode
{
  GstMPDRepresentationBaseNode parent_instance;
  /* RepresentationBase extension */
  guint level;
  guint *dependencyLevel;            /* UIntVectorType */
  guint dependencyLevel_size;        /* size of "dependencyLevel" array */
  guint bandwidth;
  gchar **contentComponent;          /* StringVectorType */
};

GstMPDSubRepresentationNode * gst_mpd_sub_representation_node_new (void);
void gst_mpd_sub_representation_node_free (GstMPDSubRepresentationNode* self);

G_END_DECLS

#endif /* __GSTMPDSUBREPRESENTATIONNODE_H__ */
