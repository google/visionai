/*
 * GStreamer
 * Copyright (C) 2013 Julien Isorce <julien.isorce@gmail.com>
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

#ifndef __GST_GL_H__
#define __GST_GL_H__

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgl_fwd.h"
#include <gst/gl/gl-enumtypes.h>
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglapi.h"
#include <gst/gl/gstglconfig.h>
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglcontext.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglcontextconfig.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgldebug.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgldisplay.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglfeature.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglformat.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglutils.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglwindow.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglslstage.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglshader.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglshaderstrings.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglcolorconvert.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglupload.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbasememory.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglmemory.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglmemorypbo.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglrenderbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbufferpool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglframebuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbasefilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbasesrc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglviewconvert.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglfilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglsyncmeta.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgloverlaycompositor.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglquery.h"

#endif /* __GST_GL_H__ */
