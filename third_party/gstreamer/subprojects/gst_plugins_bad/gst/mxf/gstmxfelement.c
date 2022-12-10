
/* GStreamer
 * Copyright (C) <2008> Sebastian Dröge <sebastian.droege@collabora.co.uk>
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

#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/gstmxfelements.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfquark.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfdemux.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfmux.h"
/*#include "mxfdms1.h"*/
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfaes-bwf.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfalaw.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfd10.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfdv-dif.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfjpeg2000.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfmpeg.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfup.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfvc3.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfprores.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst/mxf/mxfvanc.h"

GST_DEBUG_CATEGORY (mxf_debug);
#define GST_CAT_DEFAULT mxf_debug

static void
mxf_init (void)
{
  gst_tag_register (GST_TAG_MXF_UMID, GST_TAG_FLAG_META,
      G_TYPE_STRING, "UMID", "Unique Material Identifier", NULL);
  gst_tag_register (GST_TAG_MXF_STRUCTURE, GST_TAG_FLAG_META,
      GST_TYPE_STRUCTURE, "Structure", "Structural metadata of "
      "the MXF file", NULL);
  gst_tag_register (GST_TAG_MXF_DESCRIPTIVE_METADATA_FRAMEWORK,
      GST_TAG_FLAG_META, GST_TYPE_STRUCTURE, "DM Framework",
      "Descriptive metadata framework", NULL);
}

void
mxf_element_init (GstPlugin * plugin)
{
  static gsize res = FALSE;

  if (g_once_init_enter (&res)) {
    GST_DEBUG_CATEGORY_INIT (mxf_debug, "mxf", 0, "MXF");

    mxf_init ();
    mxf_quark_initialize ();
    mxf_metadata_init_types ();
    /*  mxf_dms1_initialize (); */
    mxf_aes_bwf_init ();
    mxf_alaw_init ();
    mxf_d10_init ();
    mxf_dv_dif_init ();
    mxf_jpeg2000_init ();
    mxf_mpeg_init ();
    mxf_up_init ();
    mxf_vc3_init ();
    mxf_prores_init ();
    mxf_vanc_init ();
    g_once_init_leave (&res, TRUE);
  }
}
