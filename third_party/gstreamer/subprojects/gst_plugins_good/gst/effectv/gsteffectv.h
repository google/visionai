/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 *
 * EffecTV:
 * Copyright (C) 2001 FUKUCHI Kentarou
 *
 *  EffecTV is free software. This library is free software;
 * you can redistribute it and/or
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

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

GST_ELEMENT_REGISTER_DECLARE (edgetv);
GST_ELEMENT_REGISTER_DECLARE (agingtv);
GST_ELEMENT_REGISTER_DECLARE (dicetv);
GST_ELEMENT_REGISTER_DECLARE (warptv);
GST_ELEMENT_REGISTER_DECLARE (shagadelictv);
GST_ELEMENT_REGISTER_DECLARE (vertigotv);
GST_ELEMENT_REGISTER_DECLARE (revtv);
GST_ELEMENT_REGISTER_DECLARE (quarktv);
GST_ELEMENT_REGISTER_DECLARE (optv);
GST_ELEMENT_REGISTER_DECLARE (radioactv);
GST_ELEMENT_REGISTER_DECLARE (streaktv);
GST_ELEMENT_REGISTER_DECLARE (rippletv);

static inline guint
fastrand (void)
{
  static guint fastrand_val;

  return (fastrand_val = fastrand_val * 1103515245 + 12345);
}

