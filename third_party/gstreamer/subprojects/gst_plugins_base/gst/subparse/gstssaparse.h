/* GStreamer SSA subtitle parser
 * Copyright (c) 2006 Tim-Philipp Müller <tim centricular net>
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

#ifndef __GST_SSA_PARSE_H__
#define __GST_SSA_PARSE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_BEGIN_DECLS

#define GST_TYPE_SSA_PARSE (gst_ssa_parse_get_type ())
G_DECLARE_FINAL_TYPE (GstSsaParse, gst_ssa_parse, GST, SSA_PARSE, GstElement)

struct _GstSsaParse {
  GstElement element;

  GstPad         *sinkpad;
  GstPad         *srcpad;

  gboolean        framed;
  gboolean        send_tags;

  gchar          *ini;
};

G_END_DECLS

#endif /* __GST_SSA_PARSE_H__ */

