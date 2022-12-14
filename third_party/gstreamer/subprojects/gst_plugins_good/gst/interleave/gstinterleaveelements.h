/* GStreamer interleave plugin
 * Copyright (C) 2004,2007 Andy Wingo <wingo at pobox.com>
 *
 * plugin.h: the stubs for the interleave plugin
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


#ifndef __GST_INTERLEAVE_ELEMENTS_H__
#define __GST_INTERLEAVE_ELEMENTS_H__


#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/interleave/interleave.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/interleave/deinterleave.h"

GST_ELEMENT_REGISTER_DECLARE (interleave);
GST_ELEMENT_REGISTER_DECLARE (deinterleave);

#endif /* __GST_INTERLEAVE_ELEMENTS_H__ */
