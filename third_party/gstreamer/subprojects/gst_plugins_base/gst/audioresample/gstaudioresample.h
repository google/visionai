/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 * Copyright (C) <2007-2008> Sebastian Dröge <sebastian.droege@collabora.co.uk>
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


#ifndef __AUDIO_RESAMPLE_H__
#define __AUDIO_RESAMPLE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/base/gstbasetransform.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/audio/audio.h"

G_BEGIN_DECLS

#define GST_TYPE_AUDIO_RESAMPLE (gst_audio_resample_get_type())
G_DECLARE_FINAL_TYPE (GstAudioResample, gst_audio_resample, GST, AUDIO_RESAMPLE,
    GstBaseTransform)

/**
 * GstAudioResample:
 *
 * Opaque data structure.
 */
struct _GstAudioResample {
  GstBaseTransform element;

  /* <private> */
  gboolean need_discont;

  GstClockTime t0;
  guint64 in_offset0;
  guint64 out_offset0;
  guint64 samples_in;
  guint64 samples_out;

  guint64 num_gap_samples;
  guint64 num_nongap_samples;

  /* properties */
  GstAudioResamplerMethod method;
  gint quality;
  GstAudioResamplerFilterMode sinc_filter_mode;
  guint32 sinc_filter_auto_threshold;
  GstAudioResamplerFilterInterpolation sinc_filter_interpolation;

  /* state */
  GstAudioInfo in;
  GstAudioInfo out;
  GstAudioConverter *converter;
};
GST_ELEMENT_REGISTER_DECLARE (audioresample);

G_END_DECLS

#endif /* __AUDIO_RESAMPLE_H__ */
