/*
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
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
#include "third_party/gstreamer/subprojects/gst_plugins_good/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiopanorama.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audioinvert.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiokaraoke.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audioamplify.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiodynamic.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiocheblimit.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiochebband.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audioiirfilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiowsincband.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiowsinclimit.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audiofirfilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/audioecho.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/gstscaletempo.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/audiofx/gststereo.h"

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and pad templates
 * register the features
 */
static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (audiopanorama, plugin);
  ret |= GST_ELEMENT_REGISTER (audioinvert, plugin);
  ret |= GST_ELEMENT_REGISTER (audiokaraoke, plugin);
  ret |= GST_ELEMENT_REGISTER (audioamplify, plugin);
  ret |= GST_ELEMENT_REGISTER (audiodynamic, plugin);
  ret |= GST_ELEMENT_REGISTER (audiocheblimit, plugin);
  ret |= GST_ELEMENT_REGISTER (audiochebband, plugin);
  ret |= GST_ELEMENT_REGISTER (audioiirfilter, plugin);
  ret |= GST_ELEMENT_REGISTER (audiowsinclimit, plugin);
  ret |= GST_ELEMENT_REGISTER (audiowsincband, plugin);
  ret |= GST_ELEMENT_REGISTER (audiofirfilter, plugin);
  ret |= GST_ELEMENT_REGISTER (audioecho, plugin);
  ret |= GST_ELEMENT_REGISTER (scaletempo, plugin);
  ret |= GST_ELEMENT_REGISTER (stereo, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    audiofx,
    "Audio effects plugin",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
