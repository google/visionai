/* GStreamer
 * Copyright (C) <2010> Thiago Santos <thiago.sousa.santos@collabora.co.uk>
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

#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstcircle.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstdiffuse.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstkaleidoscope.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstmarble.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstpinch.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstrotate.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstsphere.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gsttwirl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstwaterripple.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gststretch.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstbulge.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gsttunnel.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstsquare.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstmirror.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstfisheye.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/geometrictransform/gstperspective.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (circle, plugin);
  ret |= GST_ELEMENT_REGISTER (diffuse, plugin);
  ret |= GST_ELEMENT_REGISTER (kaleidoscope, plugin);
  ret |= GST_ELEMENT_REGISTER (marble, plugin);
  ret |= GST_ELEMENT_REGISTER (pinch, plugin);
  ret |= GST_ELEMENT_REGISTER (rotate, plugin);
  ret |= GST_ELEMENT_REGISTER (sphere, plugin);
  ret |= GST_ELEMENT_REGISTER (twirl, plugin);
  ret |= GST_ELEMENT_REGISTER (waterripple, plugin);
  ret |= GST_ELEMENT_REGISTER (stretch, plugin);
  ret |= GST_ELEMENT_REGISTER (bulge, plugin);
  ret |= GST_ELEMENT_REGISTER (tunnel, plugin);
  ret |= GST_ELEMENT_REGISTER (square, plugin);
  ret |= GST_ELEMENT_REGISTER (mirror, plugin);
  ret |= GST_ELEMENT_REGISTER (fisheye, plugin);
  ret |= GST_ELEMENT_REGISTER (perspective, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    geometrictransform,
    "Various geometric image transform elements",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
