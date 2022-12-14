/* GStreamer ReplayGain plugin
 *
 * Copyright (C) 2006 Rene Stadler <mail@renestadler.de>
 * 
 * replaygain.c: Plugin providing ReplayGain related elements
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifdef HAVE_CONFIG_H
#include "third_party/gstreamer/subprojects/gst_plugins_good/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/replaygain/gstrganalysis.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/replaygain/gstrglimiter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/replaygain/gstrgvolume.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (rganalysis, plugin);
  ret |= GST_ELEMENT_REGISTER (rglimiter, plugin);
  ret |= GST_ELEMENT_REGISTER (rgvolume, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR, replaygain,
    "ReplayGain volume normalization", plugin_init, VERSION, GST_LICENSE,
    GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
