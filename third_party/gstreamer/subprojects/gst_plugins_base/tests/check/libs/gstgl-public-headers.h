#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gstglfuncs.h"

#if GST_GL_HAVE_PLATFORM_EGL
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/egl/gstgldisplay_egl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/egl/gstglcontext_egl.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/egl/gstglmemoryegl.h"
#endif

#if GST_GL_HAVE_WINDOW_X11
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/x11/gstgldisplay_x11.h"
#endif

#if GST_GL_HAVE_WINDOW_WAYLAND
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/wayland/gstgldisplay_wayland.h"
#endif

