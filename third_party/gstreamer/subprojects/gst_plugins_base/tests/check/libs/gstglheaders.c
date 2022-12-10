/* GStreamer
 *
 * unit test for state changes on all elements
 *
 * Copyright (C) <2017> Julien Isorce <julien.isorce@gmail.com>
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

#ifdef HAVE_CONFIG_H
#  include "third_party/gstreamer/subprojects/gst_plugins_base/config.h"
#endif

/* This test check that public gstgl headers does not include any
 * GL headers. Except for gst/gl/gstglfuncs.h */

#include "third_party/gstreamer/subprojects/gstreamer/libs/gst/check/gstcheck.h"

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgl_enums.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglapi.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbasefilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbasememory.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbasesrc.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglbufferpool.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglcolorconvert.h"
#include <gst/gl/gstglconfig.h>
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglcontext.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgldebug.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgldisplay.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglfeature.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglfilter.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglformat.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglframebuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglmemory.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglmemorypbo.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstgloverlaycompositor.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglquery.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglrenderbuffer.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglshader.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglshaderstrings.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglsl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglslstage.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglsyncmeta.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglupload.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglutils.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglviewconvert.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglwindow.h"

#if GST_GL_HAVE_PLATFORM_EGL
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/egl/gstgldisplay_egl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/egl/gstgldisplay_egl_device.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/egl/gstglmemoryegl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/egl/gsteglimage.h"
#endif

#if GST_GL_HAVE_WINDOW_X11
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/x11/gstgldisplay_x11.h"
#endif

#if GST_GL_HAVE_WINDOW_WAYLAND
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/wayland/gstgldisplay_wayland.h"
#endif

#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gl.h"

#if defined(GLint) || defined(GLAPI) || defined(GL_GLEXT_VERSION)
#error gl headers should not be included
#endif

#if defined(EGLint) || defined(EGLBoolean) || defined(EGLAPI)
#error egl headers should not be included
#endif

static GstGLDisplay *display;
static GstGLContext *context;

static void
setup (void)
{
  display = gst_gl_display_new ();
  context = gst_gl_context_new (display);
  gst_gl_context_create (context, 0, NULL);
  gst_gl_buffer_init_once ();
  gst_gl_memory_init_once ();
  gst_gl_memory_pbo_init_once ();
  gst_gl_renderbuffer_init_once ();
}

static void
teardown (void)
{
  gst_object_unref (context);
  gst_object_unref (display);
}

GST_START_TEST (test_constructors)
{
  GstBufferPool *pool = NULL;
  GstGLColorConvert *convert = NULL;
  GstGLOverlayCompositor *compositor = NULL;
  GstGLUpload *upload = NULL;

  pool = gst_gl_buffer_pool_new (context);
  fail_if (pool == NULL);
  gst_object_unref (pool);

  convert = gst_gl_color_convert_new (context);
  fail_if (convert == NULL);
  gst_object_unref (convert);

  compositor = gst_gl_overlay_compositor_new (context);
  fail_if (compositor == NULL);
  gst_object_unref (compositor);

  upload = gst_gl_upload_new (context);
  fail_if (upload == NULL);
  gst_object_unref (upload);
}

GST_END_TEST;

static void
_construct_with_activated_context (GstGLContext * context, gpointer unused)
{
  GstGLFramebuffer *framebuffer = NULL;
  GstGLShader *shader = NULL;
  GstGLSLStage *stage = NULL;

  framebuffer = gst_gl_framebuffer_new (context);
  fail_if (framebuffer == NULL);
  gst_object_unref (framebuffer);

  shader = gst_gl_shader_new (context);
  fail_if (shader == NULL);
  gst_object_unref (shader);

  stage = gst_glsl_stage_new_default_fragment (context);
  fail_if (stage == NULL);
  gst_object_unref (stage);
}

GST_START_TEST (test_constructors_require_activated_context)
{
  gst_gl_context_thread_add (context, _construct_with_activated_context, NULL);
}

GST_END_TEST;


static Suite *
gst_gl_headers_suite (void)
{
  Suite *s = suite_create ("Gst GL Headers");
  TCase *tc_chain = tcase_create ("general");

  suite_add_tcase (s, tc_chain);
  tcase_add_checked_fixture (tc_chain, setup, teardown);
  tcase_add_test (tc_chain, test_constructors);
  tcase_add_test (tc_chain, test_constructors_require_activated_context);

  return s;
}

GST_CHECK_MAIN (gst_gl_headers);
