/* GStreamer DTMF plugin
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

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/dtmf/gstdtmfsrc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/dtmf/gstrtpdtmfsrc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/dtmf/gstrtpdtmfdepay.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (dtmfsrc, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpdtmfsrc, plugin);
  ret |= GST_ELEMENT_REGISTER (rtpdtmfdepay, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    dtmf, "DTMF plugins",
    plugin_init, VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
