/* GLIB - Library of useful routines for C programming
 * Copyright (C) 1995-1997  Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Modified by the GLib Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GLib Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GLib at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __G_LIB_H__
#define __G_LIB_H__

#define __GLIB_H_INSIDE__

#include "third_party/glib/glib/galloca.h"
#include "third_party/glib/glib/garray.h"
#include "third_party/glib/glib/gasyncqueue.h"
#include "third_party/glib/glib/gatomic.h"
#include "third_party/glib/glib/gbacktrace.h"
#include "third_party/glib/glib/gbase64.h"
#include "third_party/glib/glib/gbitlock.h"
#include "third_party/glib/glib/gbookmarkfile.h"
#include "third_party/glib/glib/gbytes.h"
#include "third_party/glib/glib/gcharset.h"
#include "third_party/glib/glib/gchecksum.h"
#include "third_party/glib/glib/gconvert.h"
#include "third_party/glib/glib/gdataset.h"
#include "third_party/glib/glib/gdate.h"
#include "third_party/glib/glib/gdatetime.h"
#include "third_party/glib/glib/gdir.h"
#include "third_party/glib/glib/genviron.h"
#include "third_party/glib/glib/gerror.h"
#include "third_party/glib/glib/gfileutils.h"
#include "third_party/glib/glib/ggettext.h"
#include "third_party/glib/glib/ghash.h"
#include "third_party/glib/glib/ghmac.h"
#include "third_party/glib/glib/ghook.h"
#include "third_party/glib/glib/ghostutils.h"
#include "third_party/glib/glib/giochannel.h"
#include "third_party/glib/glib/gkeyfile.h"
#include "third_party/glib/glib/glist.h"
#include "third_party/glib/glib/gmacros.h"
#include "third_party/glib/glib/gmain.h"
#include "third_party/glib/glib/gmappedfile.h"
#include "third_party/glib/glib/gmarkup.h"
#include "third_party/glib/glib/gmem.h"
#include "third_party/glib/glib/gmessages.h"
#include "third_party/glib/glib/gnode.h"
#include "third_party/glib/glib/goption.h"
#include "third_party/glib/glib/gpattern.h"
#include "third_party/glib/glib/gpoll.h"
#include "third_party/glib/glib/gprimes.h"
#include "third_party/glib/glib/gqsort.h"
#include "third_party/glib/glib/gquark.h"
#include "third_party/glib/glib/gqueue.h"
#include "third_party/glib/glib/grand.h"
#include "third_party/glib/glib/grcbox.h"
#include "third_party/glib/glib/grefcount.h"
#include "third_party/glib/glib/grefstring.h"
#include "third_party/glib/glib/gregex.h"
#include "third_party/glib/glib/gscanner.h"
#include "third_party/glib/glib/gsequence.h"
#include "third_party/glib/glib/gshell.h"
#include "third_party/glib/glib/gslice.h"
#include "third_party/glib/glib/gslist.h"
#include "third_party/glib/glib/gspawn.h"
#include "third_party/glib/glib/gstrfuncs.h"
#include "third_party/glib/glib/gstringchunk.h"
#include "third_party/glib/glib/gstring.h"
#include "third_party/glib/glib/gstrvbuilder.h"
#include "third_party/glib/glib/gtestutils.h"
#include "third_party/glib/glib/gthread.h"
#include "third_party/glib/glib/gthreadpool.h"
#include "third_party/glib/glib/gtimer.h"
#include "third_party/glib/glib/gtimezone.h"
#include "third_party/glib/glib/gtrashstack.h"
#include "third_party/glib/glib/gtree.h"
#include "third_party/glib/glib/gtypes.h"
#include "third_party/glib/glib/gunicode.h"
#include "third_party/glib/glib/guri.h"
#include "third_party/glib/glib/gutils.h"
#include "third_party/glib/glib/guuid.h"
#include "third_party/glib/glib/gvariant.h"
#include "third_party/glib/glib/gvarianttype.h"
#include "third_party/glib/glib/gversion.h"
#include "third_party/glib/glib/gversionmacros.h"

#ifdef G_PLATFORM_WIN32
#include <glib/gwin32.h>
#endif

#include "third_party/glib/glib/deprecated/gallocator.h"
#include "third_party/glib/glib/deprecated/gcache.h"
#include "third_party/glib/glib/deprecated/gcompletion.h"
#include "third_party/glib/glib/deprecated/gmain.h"
#include "third_party/glib/glib/deprecated/grel.h"
#include "third_party/glib/glib/deprecated/gthread.h"

#include "third_party/glib/glib/glib-autocleanups.h"
#include "third_party/glib/glib/glib-typeof.h"

#undef __GLIB_H_INSIDE__

#endif /* __G_LIB_H__ */
