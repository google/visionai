/* GStreamer
 * Copyright (C) <2007> Wim Taymans <wim@fluendo.com>
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
#include "third_party/gstreamer/subprojects/gst_plugins_bad/config.h"
#endif

#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/videosignal/gstvideoanalyse.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/videosignal/gstsimplevideomarkdetect.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/videosignal/gstsimplevideomark.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (videoanalyse, plugin);
  /* FIXME under no circumstances is anyone allowed to revive the
   * element formerly known as simplevideomarkdetect without changing the name
   * first.  XOXO  --ds  */
  ret |= GST_ELEMENT_REGISTER (simplevideomarkdetect, plugin);
  ret |= GST_ELEMENT_REGISTER (simplevideomark, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    videosignal,
    "Various video signal analysers",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
