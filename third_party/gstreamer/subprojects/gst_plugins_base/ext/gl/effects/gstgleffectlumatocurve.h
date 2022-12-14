/*
 * GStreamer
 * Copyright (C) 2008 Filippo Argiolas <filippo.argiolas@gmail.com>
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

#ifndef __GST_GL_LUMA_TO_CURVE_H__
#define __GST_GL_LUMA_TO_CURVE_H__

#include "third_party/gstreamer/subprojects/gst_plugins_base/ext/gl/effects/gstgleffectscurves.h"

G_BEGIN_DECLS

void gst_gl_effects_luma_to_curve (GstGLEffects *effects,
                                   const GstGLEffectsCurve *curve,
                                   gint curve_index,
                                   GstGLMemory *in_tex,
                                   GstGLMemory *out_tex);
G_END_DECLS

#endif /* __GST_GL_LUMA_TO_CURVE_H__ */
