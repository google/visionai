/*
 * GStreamer
 * Copyright (C) 2015 Matthew Waters <matthew@centricular.com>
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

#ifndef __GST_VULKAN_H__
#define __GST_VULKAN_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkapi.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkdebug.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkerror.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkformat.h"

/* vulkan wrapper objects */
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkinstance.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkphysicaldevice.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkdevice.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkqueue.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkfence.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkdisplay.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkwindow.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkmemory.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkbarrier.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkbuffermemory.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkimagememory.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkimageview.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkbufferpool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkimagebufferpool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkcommandbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkcommandpool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkdescriptorset.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkdescriptorpool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkhandle.h"

/* helper elements */
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkvideofilter.h"

/* helper vulkan objects */
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkdescriptorcache.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvktrash.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkswapper.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkhandlepool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkfullscreenquad.h"

#include "third_party/gstreamer/subprojects/gst_plugins_bad/gst_libs/gst/vulkan/gstvkutils.h"

#endif /* __GST_VULKAN_H__ */
