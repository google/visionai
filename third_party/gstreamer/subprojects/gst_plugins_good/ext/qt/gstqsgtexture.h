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

#ifndef __GST_QSG_TEXTURE_H__
#define __GST_QSG_TEXTURE_H__

#include "third_party/gstreamer/subprojects/gstreamer/gst/gst.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/video/video.h"
#include "third_party/gstreamer/subprojects/gst_plugins_base/gst_libs/gst/gl/gl.h"

#include "third_party/gstreamer/subprojects/gst_plugins_good/ext/qt/gstqtgl.h"
#include <QtQuick/QSGTexture>
#include <QtGui/QOpenGLFunctions>

class GstQSGTexture : public QSGTexture, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    GstQSGTexture ();
    ~GstQSGTexture ();

    void setCaps (GstCaps * caps);
    gboolean setBuffer (GstBuffer * buffer);
    GstBuffer * getBuffer (gboolean * was_bound);

    /* QSGTexture */
    void bind ();
    int textureId () const;
    QSize textureSize () const;
    bool hasAlphaChannel () const;
    bool hasMipmaps () const;

private:
    GstBuffer * buffer_;
    gboolean buffer_was_bound;
    GstBuffer * sync_buffer_;
    GWeakRef qt_context_ref_;
    GstMemory * mem_;
    GLuint dummy_tex_id_;
    GstVideoInfo v_info;
    GstVideoFrame v_frame;
};

#endif /* __GST_QSG_TEXTURE_H__ */
