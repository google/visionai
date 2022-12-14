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
#ifndef __GSTMPDADAPTATIONSETNODE_H__
#define __GSTMPDADAPTATIONSETNODE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/dash/gstmpdhelper.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/dash/gstmpdrepresentationbasenode.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/dash/gstmpdsegmentlistnode.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/dash/gstmpdsegmenttemplatenode.h"

G_BEGIN_DECLS

#define GST_TYPE_MPD_ADAPTATION_SET_NODE gst_mpd_adaptation_set_node_get_type ()
G_DECLARE_FINAL_TYPE (GstMPDAdaptationSetNode, gst_mpd_adaptation_set_node, GST, MPD_ADAPTATION_SET_NODE, GstMPDRepresentationBaseNode)

struct _GstMPDAdaptationSetNode
{
  GstMPDRepresentationBaseNode parent_instance;
  guint id;
  guint group;
  gchar *lang;                      /* LangVectorType RFC 5646 */
  gchar *contentType;
  GstXMLRatio *par;
  guint minBandwidth;
  guint maxBandwidth;
  guint minWidth;
  guint maxWidth;
  guint minHeight;
  guint maxHeight;
  GstXMLConditionalUintType *segmentAlignment;
  GstXMLConditionalUintType *subsegmentAlignment;
  GstMPDSAPType subsegmentStartsWithSAP;
  gboolean bitstreamSwitching;
  /* list of Accessibility DescriptorType nodes */
  GList *Accessibility;
  /* list of Role DescriptorType nodes */
  GList *Role;
  /* list of Rating DescriptorType nodes */
  GList *Rating;
  /* list of Viewpoint DescriptorType nodes */
  GList *Viewpoint;
  /* SegmentBase node */
  GstMPDSegmentBaseNode *SegmentBase;
  /* SegmentList node */
  GstMPDSegmentListNode *SegmentList;
  /* SegmentTemplate node */
  GstMPDSegmentTemplateNode *SegmentTemplate;
  /* list of BaseURL nodes */
  GList *BaseURLs;
  /* list of Representation nodes */
  GList *Representations;
  /* list of ContentComponent nodes */
  GList *ContentComponents;

  gchar *xlink_href;
  GstMPDXLinkActuate actuate;
};

GstMPDAdaptationSetNode * gst_mpd_adaptation_set_node_new (void);
void gst_mpd_adaptation_set_node_free (GstMPDAdaptationSetNode* self);

G_END_DECLS

#endif /* __GSTMPDADAPTATIONSETNODE_H__ */
