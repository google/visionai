/* GStreamer
 * Copyright (C) 2003 Julien Moutte <julien@moutte.net>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2008 Nokia Corporation. (contact <stefan.kost@nokia.com>)
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
 
#ifndef __RSN_INPUT_SELECTOR_H__
#define __RSN_INPUT_SELECTOR_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"

G_BEGIN_DECLS

#define GST_TYPE_INPUT_SELECTOR \
  (gst_input_selector_get_type())
#define GST_INPUT_SELECTOR(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_INPUT_SELECTOR, RsnInputSelector))
#define GST_INPUT_SELECTOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_INPUT_SELECTOR, RsnInputSelectorClass))
#define GST_IS_INPUT_SELECTOR(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_INPUT_SELECTOR))
#define GST_IS_INPUT_SELECTOR_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_INPUT_SELECTOR))

typedef struct _RsnInputSelector RsnInputSelector;
typedef struct _RsnInputSelectorClass RsnInputSelectorClass;

#define GST_INPUT_SELECTOR_GET_LOCK(sel) (&((RsnInputSelector*)(sel))->lock)
#define GST_INPUT_SELECTOR_GET_COND(sel) (&((RsnInputSelector*)(sel))->cond)
#define GST_INPUT_SELECTOR_LOCK(sel) (g_mutex_lock (GST_INPUT_SELECTOR_GET_LOCK(sel)))
#define GST_INPUT_SELECTOR_UNLOCK(sel) (g_mutex_unlock (GST_INPUT_SELECTOR_GET_LOCK(sel)))
#define GST_INPUT_SELECTOR_WAIT(sel) (g_cond_wait (GST_INPUT_SELECTOR_GET_COND(sel), \
			GST_INPUT_SELECTOR_GET_LOCK(sel)))
#define GST_INPUT_SELECTOR_BROADCAST(sel) (g_cond_broadcast (GST_INPUT_SELECTOR_GET_COND(sel)))

/**
 * RsnInputSelectorSyncMode:
 * @GST_INPUT_SELECTOR_SYNC_MODE_ACTIVE_SEGMENT: Sync using the current active segment.
 * @GST_INPUT_SELECTOR_SYNC_MODE_CLOCK: Sync using the clock.
 *
 * The different ways that input-selector can behave when in sync-streams mode.
 */
typedef enum {
  GST_INPUT_SELECTOR_SYNC_MODE_ACTIVE_SEGMENT,
  GST_INPUT_SELECTOR_SYNC_MODE_CLOCK
} RsnInputSelectorSyncMode;

struct _RsnInputSelector {
  GstElement element;

  GstPad *srcpad;

  GstPad *active_sinkpad;
  guint n_pads;
  guint padcount;
  gboolean sync_streams;
  RsnInputSelectorSyncMode sync_mode;
  gboolean cache_buffers;

  GMutex lock;
  GCond cond;
  gboolean blocked;
  gboolean flushing;
};

struct _RsnInputSelectorClass {
  GstElementClass parent_class;

  gint64 (*block)	(RsnInputSelector *self);
};

G_GNUC_INTERNAL GType gst_input_selector_get_type (void);

G_END_DECLS

#endif /* __GST_INPUT_SELECTOR_H__ */
