/* GStreamer Wayland video sink
 *
 * Copyright (C) 2014 Collabora Ltd.
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
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include "third_party/gstreamer/subprojects/gst_plugins_bad/config.h"
#endif

#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/wayland/wldisplay.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/wayland/wlbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_bad/ext/wayland/wlvideoformat.h"

#include <errno.h>

GST_DEBUG_CATEGORY_EXTERN (gstwayland_debug);
#define GST_CAT_DEFAULT gstwayland_debug

G_DEFINE_TYPE (GstWlDisplay, gst_wl_display, G_TYPE_OBJECT);

static void gst_wl_display_finalize (GObject * gobject);

static void
gst_wl_display_class_init (GstWlDisplayClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = gst_wl_display_finalize;
}

static void
gst_wl_display_init (GstWlDisplay * self)
{
  self->shm_formats = g_array_new (FALSE, FALSE, sizeof (uint32_t));
  self->dmabuf_formats = g_array_new (FALSE, FALSE, sizeof (uint32_t));
  self->wl_fd_poll = gst_poll_new (TRUE);
  self->buffers = g_hash_table_new (g_direct_hash, g_direct_equal);
  g_mutex_init (&self->buffers_mutex);
}

static void
gst_wl_ref_wl_buffer (gpointer key, gpointer value, gpointer user_data)
{
  g_object_ref (value);
}

static void
gst_wl_display_finalize (GObject * gobject)
{
  GstWlDisplay *self = GST_WL_DISPLAY (gobject);

  gst_poll_set_flushing (self->wl_fd_poll, TRUE);
  if (self->thread)
    g_thread_join (self->thread);

  /* to avoid buffers being unregistered from another thread
   * at the same time, take their ownership */
  g_mutex_lock (&self->buffers_mutex);
  self->shutting_down = TRUE;
  g_hash_table_foreach (self->buffers, gst_wl_ref_wl_buffer, NULL);
  g_mutex_unlock (&self->buffers_mutex);

  g_hash_table_foreach (self->buffers,
      (GHFunc) gst_wl_buffer_force_release_and_unref, NULL);
  g_hash_table_remove_all (self->buffers);

  g_array_unref (self->shm_formats);
  g_array_unref (self->dmabuf_formats);
  gst_poll_free (self->wl_fd_poll);
  g_hash_table_unref (self->buffers);
  g_mutex_clear (&self->buffers_mutex);

  if (self->viewporter)
    wp_viewporter_destroy (self->viewporter);

  if (self->shm)
    wl_shm_destroy (self->shm);

  if (self->dmabuf)
    zwp_linux_dmabuf_v1_destroy (self->dmabuf);

  if (self->wl_shell)
    wl_shell_destroy (self->wl_shell);

  if (self->xdg_wm_base)
    xdg_wm_base_destroy (self->xdg_wm_base);

  if (self->fullscreen_shell)
    zwp_fullscreen_shell_v1_release (self->fullscreen_shell);

  if (self->compositor)
    wl_compositor_destroy (self->compositor);

  if (self->subcompositor)
    wl_subcompositor_destroy (self->subcompositor);

  if (self->registry)
    wl_registry_destroy (self->registry);

  if (self->display_wrapper)
    wl_proxy_wrapper_destroy (self->display_wrapper);

  if (self->queue)
    wl_event_queue_destroy (self->queue);

  if (self->own_display) {
    wl_display_flush (self->display);
    wl_display_disconnect (self->display);
  }

  G_OBJECT_CLASS (gst_wl_display_parent_class)->finalize (gobject);
}

static void
shm_format (void *data, struct wl_shm *wl_shm, uint32_t format)
{
  GstWlDisplay *self = data;

  g_array_append_val (self->shm_formats, format);
}

static const struct wl_shm_listener shm_listener = {
  shm_format
};

static void
dmabuf_format (void *data, struct zwp_linux_dmabuf_v1 *zwp_linux_dmabuf,
    uint32_t format)
{
  GstWlDisplay *self = data;

  if (gst_wl_dmabuf_format_to_video_format (format) != GST_VIDEO_FORMAT_UNKNOWN)
    g_array_append_val (self->dmabuf_formats, format);
}

static const struct zwp_linux_dmabuf_v1_listener dmabuf_listener = {
  dmabuf_format,
};

gboolean
gst_wl_display_check_format_for_shm (GstWlDisplay * display,
    GstVideoFormat format)
{
  enum wl_shm_format shm_fmt;
  GArray *formats;
  guint i;

  shm_fmt = gst_video_format_to_wl_shm_format (format);
  if (shm_fmt == (enum wl_shm_format) -1)
    return FALSE;

  formats = display->shm_formats;
  for (i = 0; i < formats->len; i++) {
    if (g_array_index (formats, uint32_t, i) == shm_fmt)
      return TRUE;
  }

  return FALSE;
}

gboolean
gst_wl_display_check_format_for_dmabuf (GstWlDisplay * display,
    GstVideoFormat format)
{
  GArray *formats;
  guint i, dmabuf_fmt;

  if (!display->dmabuf)
    return FALSE;

  dmabuf_fmt = gst_video_format_to_wl_dmabuf_format (format);
  if (dmabuf_fmt == (guint) - 1)
    return FALSE;

  formats = display->dmabuf_formats;
  for (i = 0; i < formats->len; i++) {
    if (g_array_index (formats, uint32_t, i) == dmabuf_fmt)
      return TRUE;
  }

  return FALSE;
}

static void
handle_xdg_wm_base_ping (void *user_data, struct xdg_wm_base *xdg_wm_base,
    uint32_t serial)
{
  xdg_wm_base_pong (xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener xdg_wm_base_listener = {
  handle_xdg_wm_base_ping
};

static void
registry_handle_global (void *data, struct wl_registry *registry,
    uint32_t id, const char *interface, uint32_t version)
{
  GstWlDisplay *self = data;

  if (g_strcmp0 (interface, "wl_compositor") == 0) {
    self->compositor = wl_registry_bind (registry, id, &wl_compositor_interface,
        MIN (version, 4));
  } else if (g_strcmp0 (interface, "wl_subcompositor") == 0) {
    self->subcompositor =
        wl_registry_bind (registry, id, &wl_subcompositor_interface, 1);
  } else if (g_strcmp0 (interface, "wl_shell") == 0) {
    self->wl_shell = wl_registry_bind (registry, id, &wl_shell_interface, 1);
  } else if (g_strcmp0 (interface, "xdg_wm_base") == 0) {
    self->xdg_wm_base =
        wl_registry_bind (registry, id, &xdg_wm_base_interface, 1);
    xdg_wm_base_add_listener (self->xdg_wm_base, &xdg_wm_base_listener, self);
  } else if (g_strcmp0 (interface, "zwp_fullscreen_shell_v1") == 0) {
    self->fullscreen_shell = wl_registry_bind (registry, id,
        &zwp_fullscreen_shell_v1_interface, 1);
  } else if (g_strcmp0 (interface, "wl_shm") == 0) {
    self->shm = wl_registry_bind (registry, id, &wl_shm_interface, 1);
    wl_shm_add_listener (self->shm, &shm_listener, self);
  } else if (g_strcmp0 (interface, "wp_viewporter") == 0) {
    self->viewporter =
        wl_registry_bind (registry, id, &wp_viewporter_interface, 1);
  } else if (g_strcmp0 (interface, "zwp_linux_dmabuf_v1") == 0) {
    self->dmabuf =
        wl_registry_bind (registry, id, &zwp_linux_dmabuf_v1_interface, 1);
    zwp_linux_dmabuf_v1_add_listener (self->dmabuf, &dmabuf_listener, self);
  }
}

static void
registry_handle_global_remove (void *data, struct wl_registry *registry,
    uint32_t name)
{
  /* temporarily do nothing */
}

static const struct wl_registry_listener registry_listener = {
  registry_handle_global,
  registry_handle_global_remove
};

static gpointer
gst_wl_display_thread_run (gpointer data)
{
  GstWlDisplay *self = data;
  GstPollFD pollfd = GST_POLL_FD_INIT;

  pollfd.fd = wl_display_get_fd (self->display);
  gst_poll_add_fd (self->wl_fd_poll, &pollfd);
  gst_poll_fd_ctl_read (self->wl_fd_poll, &pollfd, TRUE);

  /* main loop */
  while (1) {
    while (wl_display_prepare_read_queue (self->display, self->queue) != 0)
      wl_display_dispatch_queue_pending (self->display, self->queue);
    wl_display_flush (self->display);

    if (gst_poll_wait (self->wl_fd_poll, GST_CLOCK_TIME_NONE) < 0) {
      gboolean normal = (errno == EBUSY);
      wl_display_cancel_read (self->display);
      if (normal)
        break;
      else
        goto error;
    }
    if (wl_display_read_events (self->display) == -1)
      goto error;
    wl_display_dispatch_queue_pending (self->display, self->queue);
  }

  return NULL;

error:
  GST_ERROR ("Error communicating with the wayland server");
  return NULL;
}

GstWlDisplay *
gst_wl_display_new (const gchar * name, GError ** error)
{
  struct wl_display *display;

  display = wl_display_connect (name);

  if (!display) {
    *error = g_error_new (g_quark_from_static_string ("GstWlDisplay"), 0,
        "Failed to connect to the wayland display '%s'",
        name ? name : "(default)");
    return NULL;
  } else {
    return gst_wl_display_new_existing (display, TRUE, error);
  }
}

GstWlDisplay *
gst_wl_display_new_existing (struct wl_display * display,
    gboolean take_ownership, GError ** error)
{
  GstWlDisplay *self;
  GError *err = NULL;
  gint i;

  g_return_val_if_fail (display != NULL, NULL);

  self = g_object_new (GST_TYPE_WL_DISPLAY, NULL);
  self->display = display;
  self->display_wrapper = wl_proxy_create_wrapper (display);
  self->own_display = take_ownership;

  self->queue = wl_display_create_queue (self->display);
  wl_proxy_set_queue ((struct wl_proxy *) self->display_wrapper, self->queue);
  self->registry = wl_display_get_registry (self->display_wrapper);
  wl_registry_add_listener (self->registry, &registry_listener, self);

  /* we need exactly 2 roundtrips to discover global objects and their state */
  for (i = 0; i < 2; i++) {
    if (wl_display_roundtrip_queue (self->display, self->queue) < 0) {
      *error = g_error_new (g_quark_from_static_string ("GstWlDisplay"), 0,
          "Error communicating with the wayland display");
      g_object_unref (self);
      return NULL;
    }
  }

  /* verify we got all the required interfaces */
#define VERIFY_INTERFACE_EXISTS(var, interface) \
  if (!self->var) { \
    g_set_error (error, g_quark_from_static_string ("GstWlDisplay"), 0, \
        "Could not bind to " interface ". Either it is not implemented in " \
        "the compositor, or the implemented version doesn't match"); \
    g_object_unref (self); \
    return NULL; \
  }

  VERIFY_INTERFACE_EXISTS (compositor, "wl_compositor");
  VERIFY_INTERFACE_EXISTS (subcompositor, "wl_subcompositor");
  VERIFY_INTERFACE_EXISTS (shm, "wl_shm");

#undef VERIFY_INTERFACE_EXISTS

  /* We make the viewporter optional even though it may cause bad display.
   * This is so one can test wayland display on older compositor or on
   * compositor that don't implement this extension. */
  if (!self->viewporter) {
    g_warning ("Wayland compositor is missing the ability to scale, video "
        "display may not work properly.");
  }

  if (!self->dmabuf) {
    g_warning ("Could not bind to zwp_linux_dmabuf_v1");
  }

  if (!self->wl_shell && !self->xdg_wm_base && !self->fullscreen_shell) {
    /* If wl_surface and wl_display are passed via GstContext
     * wl_shell, xdg_shell and zwp_fullscreen_shell are not used.
     * In this case is correct to continue.
     */
    g_warning ("Could not bind to either wl_shell, xdg_wm_base or "
        "zwp_fullscreen_shell, video display may not work properly.");
  }

  self->thread = g_thread_try_new ("GstWlDisplay", gst_wl_display_thread_run,
      self, &err);
  if (err) {
    g_propagate_prefixed_error (error, err,
        "Failed to start thread for the display's events");
    g_object_unref (self);
    return NULL;
  }

  return self;
}

void
gst_wl_display_register_buffer (GstWlDisplay * self, gpointer gstmem,
    gpointer wlbuffer)
{
  g_assert (!self->shutting_down);

  GST_TRACE_OBJECT (self, "registering GstWlBuffer %p to GstMem %p",
      wlbuffer, gstmem);

  g_mutex_lock (&self->buffers_mutex);
  g_hash_table_replace (self->buffers, gstmem, wlbuffer);
  g_mutex_unlock (&self->buffers_mutex);
}

gpointer
gst_wl_display_lookup_buffer (GstWlDisplay * self, gpointer gstmem)
{
  gpointer wlbuffer;
  g_mutex_lock (&self->buffers_mutex);
  wlbuffer = g_hash_table_lookup (self->buffers, gstmem);
  g_mutex_unlock (&self->buffers_mutex);
  return wlbuffer;
}

void
gst_wl_display_unregister_buffer (GstWlDisplay * self, gpointer gstmem)
{
  GST_TRACE_OBJECT (self, "unregistering GstWlBuffer owned by %p", gstmem);

  g_mutex_lock (&self->buffers_mutex);
  if (G_LIKELY (!self->shutting_down))
    g_hash_table_remove (self->buffers, gstmem);
  g_mutex_unlock (&self->buffers_mutex);
}
