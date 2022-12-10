/* GStreamer
 * Copyright (C) 2021 GStreamer developers
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

#pragma once

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_BEGIN_DECLS

typedef struct _GstVaDisplay GstVaDisplay;
typedef struct _GstVaDisplayClass GstVaDisplayClass;

typedef struct _GstVaDisplayDrm GstVaDisplayDrm;
typedef struct _GstVaDisplayDrmClass GstVaDisplayDrmClass;

typedef struct _GstVaDisplayWrapped GstVaDisplayWrapped;
typedef struct _GstVaDisplayWrappedClass GstVaDisplayWrappedClass;

G_END_DECLS