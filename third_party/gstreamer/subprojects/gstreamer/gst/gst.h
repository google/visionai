/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *
 * gst.h: Main header for GStreamer, apps should include this
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


#ifndef __GST_H__
#define __GST_H__

#include "third_party/glib/glib/glib.h"

#include "third_party/gstreamer/subprojects/gstreamer/gst/glib-compat.h"

#include "third_party/gstreamer/subprojects/gstreamer/gst/gstenumtypes.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstversion.h"

#include "third_party/gstreamer/subprojects/gstreamer/gst/gstatomicqueue.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstbin.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstbuffer.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstbufferlist.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstbufferpool.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstcaps.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstcapsfeatures.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstchildproxy.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstclock.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstcontrolsource.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstdatetime.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstdebugutils.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstdevice.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstdevicemonitor.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstdeviceprovider.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstdynamictypefactory.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstelement.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstelementmetadata.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsterror.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstevent.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstghostpad.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstinfo.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstiterator.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstmessage.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstmemory.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstmeta.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstminiobject.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstobject.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gststreamcollection.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstpad.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstparamspecs.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstpipeline.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstplugin.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstpoll.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstpreset.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstprotection.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstquery.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstregistry.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstpromise.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstsample.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstsegment.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gststreams.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gststructure.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstsystemclock.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttaglist.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttagsetter.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttask.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttaskpool.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttoc.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttocsetter.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttracer.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttracerfactory.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttracerrecord.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttypefind.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsttypefindfactory.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gsturi.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstutils.h"
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstvalue.h"

#include "third_party/gstreamer/subprojects/gstreamer/gst/gstparse.h"

/* API compatibility stuff */
#include "third_party/gstreamer/subprojects/gstreamer/gst/gstcompat.h"

G_BEGIN_DECLS

GST_API
void		gst_init			(int *argc, char **argv[]);

GST_API
gboolean	gst_init_check			(int *argc, char **argv[],
						 GError ** error);
GST_API
gboolean        gst_is_initialized              (void);

GST_API
GOptionGroup *	gst_init_get_option_group	(void);

GST_API
void		gst_deinit			(void);

GST_API
void		gst_version			(guint *major, guint *minor,
						 guint *micro, guint *nano);
GST_API
gchar *		gst_version_string		(void);

GST_API
gboolean        gst_segtrap_is_enabled          (void);

GST_API
void            gst_segtrap_set_enabled         (gboolean enabled);

GST_API
gboolean        gst_registry_fork_is_enabled    (void);

GST_API
void            gst_registry_fork_set_enabled   (gboolean enabled);

GST_API
gboolean        gst_update_registry             (void);

GST_API
const gchar *   gst_get_main_executable_path    (void);

G_END_DECLS

#endif /* __GST_H__ */
