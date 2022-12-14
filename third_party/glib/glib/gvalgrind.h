/*
 * Copyright 2018 Collabora ltd.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Xavier Claessens <xavier.claessens@collabora.com>
 */

#ifndef __G_VALGRIND_H__
#define __G_VALGRIND_H__

#if HAVE_STDINT_H
#include <stdint.h>
#endif

#ifndef _MSC_VER
#include "third_party/glib/glib/valgrind.h"
#define ENABLE_VALGRIND 1
#endif

#endif /* __G_VALGRIND_H__ */
