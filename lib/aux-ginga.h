/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef AUX_GINGA_H
#define AUX_GINGA_H

#include "ginga.h"

GINGA_BEGIN_DECLS

#include <config.h>
#include "aux-glib.h"

#define GINGA_PRAGMA_DIAG         PRAGMA_DIAG
#define GINGA_PRAGMA_DIAG_PUSH    PRAGMA_DIAG_PUSH
#define GINGA_PRAGMA_DIAG_POP     PRAGMA_DIAG_POP
#define GINGA_PRAGMA_DIAG_IGNORE  PRAGMA_DIAG_IGNORE
#define GINGA_PRAGMA_DIAG_WARNING PRAGMA_DIAG_WARNING

// C library.
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

// External C libraries.
#include <gdk/gdk.h>
#include <ncluaw.h>
#include <pango/pangocairo.h>

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wcast-qual)
GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>
GINGA_PRAGMA_DIAG_POP ()

#if defined WITH_LIBRSVG && WITH_LIBRSVG
# include <librsvg/rsvg.h>
#endif

GINGA_END_DECLS

// C++ library.
#include <algorithm>
#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// Namespaces.
#define _GINGA_NS_BEGIN(t)     namespace t {/*}*/
#define _GINGA_NS_END                    /*{*/}
#define _GINGA_BEGIN(t)       _GINGA_NS_BEGIN (ginga) _GINGA_NS_BEGIN (t)
#define _GINGA_END            _GINGA_NS_END _GINGA_NS_END
#define GINGA_BEGIN           _GINGA_NS_BEGIN (ginga)
#define GINGA_END             _GINGA_NS_END
#define GINGA_FORMATTER_BEGIN _GINGA_BEGIN (formatter)
#define GINGA_FORMATTER_END   _GINGA_END
#define GINGA_PLAYER_BEGIN    _GINGA_BEGIN (player)
#define GINGA_PLAYER_END      _GINGA_END

// Utility macros.
#undef unused
#define unused(...) G_GNUC_UNUSED __VA_ARGS__
#undef cast
#define cast(a, b)       (dynamic_cast<a>((b)))
#define instanceof(a, b) (cast (a,(b)) != nullptr)

// Message logging.
#define GINGA_STRLOC  __FILE__ ":" G_STRINGIFY (__LINE__) ":" GINGA_STRFUNC
#define GINGA_STRFUNC (__ginga_strfunc (G_STRFUNC)).c_str ()
string __ginga_strfunc (const string &);

#define __ginga_log(fn, fmt, ...)\
  fn ("%s: " fmt, GINGA_STRFUNC, ## __VA_ARGS__)

#define TRACE(fmt, ...)    __ginga_log (g_debug, fmt, ## __VA_ARGS__)
#define WARNING(fmt, ...)  __ginga_log (g_warning, fmt, ## __VA_ARGS__)
#define ERROR(fmt, ...)    __ginga_log (g_error, fmt, ## __VA_ARGS__)
#define CRITICAL(fmt, ...) __ginga_log (g_critical, fmt, ## __VA_ARGS__)

#define WARNING_SYNTAX(fmt, ...)\
  WARNING ("bad syntax: " fmt, ## __VA_ARGS__)

#define ERROR_SYNTAX(fmt, ...)\
  ERROR ("bad syntax: " fmt, ## __VA_ARGS__)

#define WARNING_NOT_IMPLEMENTED(fmt, ...)\
  WARNING ("not implemented: " fmt, ## __VA_ARGS__)

#define ERROR_NOT_IMPLEMENTED(fmt, ...)\
  ERROR ("not implemented: " fmt, ## __VA_ARGS__)

// Aliases.
typedef GdkRGBA GingaColor;
typedef GdkRectangle GingaRect;
typedef GstClockTime GingaTime;

// Time macros and functions.
#define ginga_gettime() ((GingaTime)(g_get_monotonic_time () * 1000))
#define GINGA_TIME_NONE            GST_CLOCK_TIME_NONE
#define GINGA_TIME_IS_VALID(t)     GST_CLOCK_TIME_IS_VALID ((t))
#define GINGA_STIME_NONE           GST_CLOCK_STIME_NONE
#define GINGA_STIME_IS_VALID(t)    GST_CLOCK_STIME_IS_VALID ((t))
#define GINGA_SECOND               GST_SECOND
#define GINGA_MSECOND              GST_MSECOND
#define GINGA_USECOND              GST_USECOND
#define GINGA_NSECOND              GST_NSECOND
#define GINGA_TIME_AS_SECONDS(t)   GST_TIME_AS_SECONDS ((t))
#define GINGA_TIME_AS_MSECONDS(t)  GST_TIME_AS_MSECONDS ((t))
#define GINGA_TIME_AS_USECONDS(t)  GST_TIME_AS_USECONDS ((t))
#define GINGA_TIME_AS_NSECONDS(t)  GST_TIME_AS_NSECONDS ((t))
#define GINGA_TIME_DIFF(s,e)       GST_CLOCK_DIFF ((s), (e))
#define GINGA_TIME_FORMAT          GST_TIME_FORMAT
#define GINGA_TIME_ARGS(t)         GST_TIME_ARGS ((t))
#define GINGA_STIME_FORMAT         GST_STIME_FORMAT
#define GINGA_STIME_ARGS(t)        GST_STIME_ARGS ((t))

// Parsing and evaluation functions.
bool _ginga_parse_bool (const string &, bool *);
bool ginga_parse_bool (const string &);
bool _ginga_parse_color (const string &, GingaColor *);
GingaColor ginga_parse_color (const string &);
bool _ginga_parse_list (const string &, char, size_t, size_t, vector<string> *);
vector<string> ginga_parse_list (const string &, char, size_t, size_t);
int ginga_parse_percent (const string &, int, int, int);
#define ginga_parse_pixel(s) (guint8) ginga_parse_percent ((s), 255, 0, 255)
bool _ginga_parse_time (const string &, GingaTime *);
GingaTime ginga_parse_time (const string &);

// String functions.
bool _xstrtod (const string &, double *);
bool _xstrtoll (const string &, gint64 *, guint);
bool _xstrtoull (const string &, guint64 *, guint);
double xstrtod (const string &);
gint xstrtoint (const string &, guint8);
gint8 xstrtoint8 (const string &, guint8);
gint64 xstrtoint64 (const string &, guint8);
guint xstrtouint (const string &, guint8);
guint8 xstrtouint8 (const string &, guint8);
guint64 xstrtouint64 (const string &, guint8);
bool xstrispercent (const string &);
double xstrtodorpercent (const string &, bool *);
int xstrcasecmp (const string &, const string &);
#define xstrcaseeq(s1, s2) (xstrcasecmp ((s1), (s2)) == 0)
bool xstrhasprefix (const string &, const string &);
bool xstrhassuffix (const string &, const string &);
int G_GNUC_PRINTF (2,3) xstrassign (string &, const char *, ...);
string G_GNUC_PRINTF (1,2) xstrbuild (const char *, ...);
string xstrup (string);
string xstrdown (string);
string xstrstrip (string);
vector<string> xstrsplit (const string &, char);

// Path functions.
string xpathbasename (string);
string xpathdirname (string);
bool xpathisabs (const string &);
bool xpathisuri (const string &);
string xpathmakeabs (string);
string xpathbuild (const string &, const string &);
string xpathbuildabs (const string &, const string &);

// Vector functions.

/**
 * @brief Removes element from vector.
 * @param v Vector.
 * @param x Element to be removed.
 */
template <typename T> void
xvectremove (vector <T> &v, T &x)
{
  v.erase (remove (v.begin (), v.end (), x), v.end ());
}

/**
 * @brief Removes from vector all elements that appear in another vector.
 * @param v1 Vector.
 * @param v2 Vector containing the elements that should be removed.
 */
template <typename T> void
xvectremove (vector<T> &v1, vector<T> &v2)
{
  auto fn
    = [&](T t) {return (find (v2.begin (), v2.end (), t) != v2.end ());};
  auto begin = remove_if (v1.begin(), v1.end(), fn);
  v1.erase (begin, v1.end());
}

#endif // AUX_GINGA_H
