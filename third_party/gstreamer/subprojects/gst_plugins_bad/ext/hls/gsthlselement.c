
#ifdef HAVE_CONFIG_H
#  include "third_party/gstreamer/subprojects/gst_plugins_bad/config.h"
#endif

#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/hls/gsthlselements.h"

GST_DEBUG_CATEGORY (hls_debug);

void
hls_element_init (GstPlugin * plugin)
{
  static gsize res = FALSE;
  if (g_once_init_enter (&res)) {
    GST_DEBUG_CATEGORY_INIT (hls_debug, "hls", 0, "HTTP Live Streaming (HLS)");
    g_once_init_leave (&res, TRUE);
  }
}
