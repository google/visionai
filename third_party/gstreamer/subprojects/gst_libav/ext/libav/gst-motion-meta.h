#ifndef THIRD_PARTY_GSTREAMER_SUBPROJECTS_GST_LIBAV_EXT_LIBAV_GST_MOTION_META_H_
#define THIRD_PARTY_GSTREAMER_SUBPROJECTS_GST_LIBAV_EXT_LIBAV_GST_MOTION_META_H_

#ifndef __GST_MOTION_META_H__
#define __GST_MOTION_META_H__

#include "third_party/ffmpeg/libavutil/motion_vector.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_BEGIN_DECLS

typedef struct _GstMotionMeta GstMotionMeta;

/**
 * GstMotionMeta:
 * @meta: parent #GstMeta
 * @mvs: (array length=size): The movion vectors associated with a single
 * AVFrame.
 * @size: The size in bytes of @mvs
 *
 * Extra buffer metadata providing motion vectors.
 */
struct _GstMotionMeta {
  GstMeta meta;

  gsize size;
  AVMotionVector *mvs;
};

GType gst_motion_meta_api_get_type(void);
#define GST_MOTION_META_API_TYPE (gst_motion_meta_api_get_type())

#define gst_buffer_get_motion_meta(b) \
  ((GstMotionMeta *)gst_buffer_get_motion_meta((b), GST_MOTION_META_API_TYPE))

/* implementation */
const GstMetaInfo *gst_motion_meta_get_info(void);
#define GST_MOTION_META_INFO (gst_motion_meta_get_info())

GstMotionMeta *gst_buffer_add_motion_meta(GstBuffer *buffer,
                                          const AVMotionVector *mvs,
                                          gsize size);
G_END_DECLS

#endif /* __GST_MOTION_META_H__ */

#endif  // THIRD_PARTY_GSTREAMER_SUBPROJECTS_GST_LIBAV_EXT_LIBAV_GST_MOTION_META_H_
