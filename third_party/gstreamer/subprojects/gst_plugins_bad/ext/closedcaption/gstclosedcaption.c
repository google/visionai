/*
 * GStreamer
 * Copyright (C) 2018 Edward Hervey <edward@centricular.com>
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
#  include "third_party/gstreamer/subprojects/gst_plugins_bad/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/closedcaption/gstcccombiner.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/closedcaption/gstccconverter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/closedcaption/gstccextractor.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/closedcaption/gstline21dec.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/closedcaption/gstceaccoverlay.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/closedcaption/gstline21enc.h"

static gboolean
closedcaption_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (cccombiner, plugin);
  ret |= GST_ELEMENT_REGISTER (ccconverter, plugin);
  ret |= GST_ELEMENT_REGISTER (ccextractor, plugin);
  ret |= GST_ELEMENT_REGISTER (line21decoder, plugin);
  ret |= GST_ELEMENT_REGISTER (cc708overlay, plugin);
  ret |= GST_ELEMENT_REGISTER (line21encoder, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    closedcaption,
    "Closed Caption elements",
    closedcaption_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
