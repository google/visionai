/* GStreamer
 * Copyright (C) <2009> Sebastian Dröge <sebastian.droege@collabora.co.uk>
 *           (C) 2010 Olivier Aubert <olivier.aubert@liris.cnrs.fr>
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

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/rsvg/gstrsvgdec.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/rsvg/gstrsvgoverlay.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (rsvgoverlay, plugin);
  ret |= GST_ELEMENT_REGISTER (rsvgdec, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    rsvg,
    "RSVG plugin library",
    plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
