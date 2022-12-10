/* GStreamer
 * Copyright (C) 2011 David Schleef <ds@entropywave.com>
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
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */

#ifdef HAVE_CONFIG_H
#include "third_party/gstreamer/subprojects/gst_plugins_bad/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbasetransform.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/videofilters/gstscenechange.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/videofilters/gstzebrastripe.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/videofilters/gstvideodiff.h"


static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (scenechange, plugin);
  ret |= GST_ELEMENT_REGISTER (zebrastripe, plugin);
  ret |= GST_ELEMENT_REGISTER (videodiff, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    videofiltersbad,
    "Video filters in gst-plugins-bad",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)
