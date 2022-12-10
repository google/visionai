#ifdef HAVE_CONFIG_H
#  include "third_party/gstreamer/subprojects/gst_plugins_bad/config.h"
#endif

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/rawparse/gstaudioparse.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/rawparse/gstvideoparse.h"

static gboolean
plugin_init (GstPlugin * plugin)
{
  gboolean ret = FALSE;

  ret |= GST_ELEMENT_REGISTER (videoparse, plugin);
  ret |= GST_ELEMENT_REGISTER (audioparse, plugin);

  return ret;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    legacyrawparse,
    "Parses byte streams into raw frames",
    plugin_init, VERSION, "LGPL", GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN);
