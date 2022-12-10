/*
 *  lines.h
 *  iGoom
 *
 *  Created by guillaum on Tue Aug 14 2001.
 *  Copyright (c) 2001 ios. All rights reserved.
 *
 */
#include "third_party/glib/glib/glib.h"

#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/goom2k1/graphic.h"
#include "third_party/gstreamer/subprojects/gst_plugins_good/gst/goom2k1/goom_core.h"

void goom_lines(GoomData *goomdata, gint16 data [2][512], unsigned int ID,unsigned int* p, guint32 power);


