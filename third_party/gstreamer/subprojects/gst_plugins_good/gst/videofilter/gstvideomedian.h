/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
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


#ifndef __GST_VIDEO_MEDIAN_H__
#define __GST_VIDEO_MEDIAN_H__


#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/gstvideofilter.h"

G_BEGIN_DECLS

#define GST_TYPE_VIDEO_MEDIAN \
  (gst_video_median_get_type())
#define GST_VIDEO_MEDIAN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_VIDEO_MEDIAN,GstVideoMedian))
#define GST_VIDEO_MEDIAN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_VIDEO_MEDIAN,GstVideoMedianClass))
#define GST_IS_VIDEO_MEDIAN(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_VIDEO_MEDIAN))
#define GST_IS_VIDEO_MEDIAN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_VIDEO_MEDIAN))

typedef struct _GstVideoMedian GstVideoMedian;
typedef struct _GstVideoMedianClass GstVideoMedianClass;

typedef enum
{
  GST_VIDEO_MEDIAN_SIZE_5 = 5,
  GST_VIDEO_MEDIAN_SIZE_9 = 9
} GstVideoMedianSize;

struct _GstVideoMedian {
  GstVideoFilter parent;

  GstVideoMedianSize filtersize;
  gboolean lum_only;
};

struct _GstVideoMedianClass {
  GstVideoFilterClass parent_class;
};

GType gst_video_median_get_type (void);

GST_ELEMENT_REGISTER_DECLARE (videomedian);

G_END_DECLS

#endif /* __GST_VIDEO_MEDIAN_H__ */
