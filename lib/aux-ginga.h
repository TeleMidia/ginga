/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

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
#include <string.h>
#include <math.h>
#include <stdlib.h>

// External C libraries.
#include <gdk/gdk.h>

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wcast-qual)
GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
#include <gst/app/gstappsink.h>
#include <gst/gst.h>
#include <gst/video/video.h>
GINGA_PRAGMA_DIAG_POP ()

GINGA_END_DECLS

// C++ library.
#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

// Namespaces.
#define GINGA_NAMESPACE_BEGIN namespace ginga {/*}*/
#define GINGA_NAMESPACE_END   /*{*/}

GINGA_NAMESPACE_BEGIN

// Utility macros.
#undef unused
#define unused(...) G_GNUC_UNUSED __VA_ARGS__

#undef cast
#define cast(a, b)       (dynamic_cast<a>((b)))
#define instanceof(a, b) (cast (a,(b)) != nullptr)

#define tryinsert(a, b, fn)                             \
  (std::count ((b).begin (), (b).end (), (a)) == 0)     \
  ? ((b).fn (a), true) : false

#define MAP_GET_IMPL(m, a, b)                   \
  G_STMT_START                                  \
  {                                             \
    auto it = (m).find ((a));                   \
    if (it == (m).end ())                       \
      return false;                             \
    tryset ((b), it->second);                   \
    return true;                                \
  }                                             \
  G_STMT_END                                    \

#define MAP_SET_IMPL(m, a, b)                           \
  G_STMT_START                                          \
  {                                                     \
    auto it = (m).find ((a));                           \
    (m)[(a)] = (b);                                     \
    return it == (m).end ();                            \
  }                                                     \
  G_STMT_END

// Logging, warnings and errors.
#define GINGA_STRFUNC (__ginga_strfunc (G_STRFUNC)).c_str ()
string __ginga_strfunc (const string &);
#define __ginga_log(fn, fmt, ...)\
  fn ("%s: " fmt, GINGA_STRFUNC, ## __VA_ARGS__)

#define TRACE(fmt, ...)    __ginga_log (g_debug, fmt, ## __VA_ARGS__)
#define WARNING(fmt, ...)  __ginga_log (g_warning, fmt, ## __VA_ARGS__)
#define ERROR(fmt, ...)    __ginga_log (g_error, fmt, ## __VA_ARGS__)
#define CRITICAL(fmt, ...) __ginga_log (g_critical, fmt, ## __VA_ARGS__)

#define ERROR_NOT_IMPLEMENTED(fmt, ...)\
  ERROR ("not implemented: " fmt, ## __VA_ARGS__)

// Internal types.
typedef GdkRGBA Color;
typedef GdkRectangle Rect;
typedef GstClockTime Time;

// Time macros and functions.
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

// Numeric functions.
bool floateq (double, double, double epsilon=.0000001);
#define doubleeq floateq

// Parsing and evaluation functions.
bool try_parse_bool (const string &, bool *);
bool parse_bool (const string &);

bool try_parse_color (const string &, Color *);
Color parse_color (const string &);

bool try_parse_list (const string &, char, size_t, size_t, list<string> *);
list<string> parse_list (const string &, char, size_t, size_t);

bool try_parse_table (const string &, map<string, string> *);
map<string, string> parse_table (const string &);

bool try_parse_time (const string &, Time *);
Time parse_time (const string &);

int parse_percent (const string &, int, int, int);
guint8 parse_pixel (const string &);

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
string xstrstrip (string);
list<string> xstrsplit (const string &, char);

// Path functions.
string xpathbasename (string);
string xpathdirname (string);
bool xpathisabs (const string &);
bool xpathisuri (const string &);
string xpathmakeabs (string);
string xpathbuild (const string &, const string &);
string xpathbuildabs (const string &, const string &);

// Uri functions.
string xpathfromuri (const string &);
string xurifromsrc (const string &, const string &);
bool xurigetcontents (const string &, string &);


// User data.
typedef void (*UserDataCleanFunc)(void *);
class UserData
{
public:
  UserData ();
  ~UserData ();
  bool getData (const string &, void **);
  bool setData (const string &, void *, UserDataCleanFunc fn=nullptr);
private:
  map<string,pair<void *, void (*)(void *)>> _udata;
};

GINGA_NAMESPACE_END
using namespace ginga;

#endif // AUX_GINGA_H
