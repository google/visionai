/* GStreamer
 * Copyright (C) 2013 FIXME <fixme@example.com>
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

#ifndef _GST_COMB_DETECT_H_
#define _GST_COMB_DETECT_H_

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideofilter.h"

G_BEGIN_DECLS

#define GST_TYPE_COMB_DETECT   (gst_comb_detect_get_type())
#define GST_COMB_DETECT(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_COMB_DETECT,GstCombDetect))
#define GST_COMB_DETECT_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_COMB_DETECT,GstCombDetectClass))
#define GST_IS_COMB_DETECT(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_COMB_DETECT))
#define GST_IS_COMB_DETECT_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_COMB_DETECT))

typedef struct _GstCombDetect GstCombDetect;
typedef struct _GstCombDetectClass GstCombDetectClass;

struct _GstCombDetect
{
  GstVideoFilter base_combdetect;

  GstVideoInfo vinfo;
};

struct _GstCombDetectClass
{
  GstVideoFilterClass base_combdetect_class;
};

GType gst_comb_detect_get_type (void);

GST_ELEMENT_REGISTER_DECLARE (combdetect);

G_END_DECLS

#endif
