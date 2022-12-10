/* iSAC plugin utils
 *
 * Copyright (C) 2020 Collabora Ltd.
 *  Author: Guillaume Desmottes <guillaume.desmottes@collabora.com>, Collabora Ltd.
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

#ifndef __GST_ISAC_UTILS_H__
#define __GST_ISAC_UTILS_H__

#include "third_party/glib/glib/glib.h"

G_BEGIN_DECLS

const gchar * isac_error_code_to_str (gint code);

#define CHECK_ISAC_RET(ret, function) \
  if (ret == -1) {\
    gint16 code = WebRtcIsac_GetErrorCode (self->isac);\
    GST_WARNING_OBJECT (self, "WebRtcIsac_"#function " call failed: %s (%d)", isac_error_code_to_str (code), code);\
    return FALSE;\
  }

G_END_DECLS

#endif /* __GST_ISAC_UTILS_H__ */
