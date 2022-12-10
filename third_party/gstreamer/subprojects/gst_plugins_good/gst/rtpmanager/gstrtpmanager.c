/* GStreamer
 * Copyright (C) <2007> Wim Taymans <wim.taymans@gmail.com>
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

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpbin.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpjitterbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpptdemux.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpsession.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtprtxqueue.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtprtxreceive.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtprtxsend.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpssrcdemux.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpdtmfmux.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpmux.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpfunnel.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpst2022-1-fecdec.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtpst2022-1-fecenc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtphdrext-twcc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/rtpmanager/gstrtphdrext-clientaudiolevel.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (rtpbin, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpjitterbuffer, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpptdemux, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpsession, plugin);
  ret |= GST_ELEMENT_REGISTER (rtprtxqueue, plugin);
  ret |= GST_ELEMENT_REGISTER (rtprtxreceive, plugin);
  ret |= GST_ELEMENT_REGISTER (rtprtxsend, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpssrcdemux, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpmux, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpdtmfmux, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpfunnel, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpst2022_1_fecdec, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpst2022_1_fecenc, plugin);
  ret |= GST_ELEMENT_REGISTER (rtphdrexttwcc, plugin);
  ret |= GST_ELEMENT_REGISTER (rtphdrextclientaudiolevel, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR, GST_VERSION_MINOR, rtpmanager,
    "RTP session management plugin library", plugin_init, VERSION, "LGPL",
    GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
