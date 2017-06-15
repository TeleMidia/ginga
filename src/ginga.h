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

#ifndef GINGA_H
#define GINGA_H

#if defined __GNUC__ && defined __GNUC_MINOR__
# define GINGA_GNUC_PREREQ(major, minor)\
   ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((major) << 16) + (minor))
#else
# define GINGA_GNUC_PREREQ(major, minor) 0
#endif

#if GINGA_GNUC_PREREQ(4,2)
# define _GCC_PRAGMA(x) _Pragma (#x)
# define GINGA_PRAGMA_DIAG(x) _GCC_PRAGMA (GCC diagnostic x)
#elif defined (__clang__)
# define _CLANG_PRAGMA(x) _Pragma (#x))
# define GINGA_PRAGMA_DIAG(x) _CLANG_PRAGMA (clang diagnostic x)
#else
# define GINGA_PRAGMA_DIAG(x)
#endif

#if GINGA_GNUC_PREREQ(4,6) || defined (__clang__)
# define GINGA_PRAGMA_DIAG_PUSH()    GINGA_PRAGMA_DIAG (push)
# define GINGA_PRAGMA_DIAG_POP()     GINGA_PRAGMA_DIAG (pop)
#else
# define GINGA_PRAGMA_DIAG_PUSH()
# define GINGA_PRAGMA_DIAG_POP()
#endif
#define GINGA_PRAGMA_DIAG_IGNORE(x)  GINGA_PRAGMA_DIAG (ignored #x)
#define GINGA_PRAGMA_DIAG_WARNING(x) GINGA_PRAGMA_DIAG (warning #x))

#ifdef  __cplusplus
# define GINGA_BEGIN_DECLS extern "C" {/*}*/
# define GINGA_END_DECLS            /*{*/}
#else
# define GINGA_BEGIN_DECLS
# define GINGA_END_DECLS
#endif

GINGA_BEGIN_DECLS

#include <config.h>

// C library.
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

// External C libraries.
#include <glib.h>
#include <glib/gstdio.h>
#include <ncluaw.h>
#include "ginga-sdlx.h"

#if defined WITH_PANGO && WITH_PANGO
# include <cairo.h>
# include <pango/pangocairo.h>
#endif

#if defined WITH_LIBRSVG && WITH_LIBRSVG
# include <cairo.h>
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
#define GINGA_FORMATTER_BEGIN _GINGA_BEGIN (formatter)
#define GINGA_FORMATTER_END   _GINGA_END
#define GINGA_MB_BEGIN        _GINGA_BEGIN (mb)
#define GINGA_MB_END          _GINGA_END
#define GINGA_NCL_BEGIN       _GINGA_BEGIN (ncl)
#define GINGA_NCL_END         _GINGA_END
#define GINGA_PLAYER_BEGIN    _GINGA_BEGIN (player)
#define GINGA_PLAYER_END      _GINGA_END

// Utility macros.
#define GINGA_ASSERT_GLOBAL_NONNULL(G, Type)                    \
  ((G != NULL) ? (G)                                            \
   : (g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,                \
             "global variable %s is null", G_STRINGIFY (G)),    \
      ((Type) NULL)))

#define arg_unused(...) G_GNUC_UNUSED __VA_ARGS__
#define set_if_nonnull(a, x) G_STMT_START {if (a) *(a) = (x); } G_STMT_END
#define likely(cond)    G_LIKELY ((cond))
#define unlikely(cond)  G_UNLIKELY ((cond))
#define deconst(t, x)   ((t)(ptrdiff_t)(const void *)(x))
#define pointerof(p)    ((void *)((ptrdiff_t)(p)))
#define streq(a,b)      (g_strcmp0 ((a),(b)) == 0)
#define xnumeq(a,b)     (ABS ((a) - (b)) <= .0000001);

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

// Thread.
#define GINGA_MUTEX_DEFN()                      \
  GRecMutex mutex;                              \
  void inline mutexInit ()                      \
  {                                             \
    g_rec_mutex_init (&this->mutex);            \
  }                                             \
  void inline mutexClear ()                     \
  {                                             \
    g_rec_mutex_clear (&this->mutex);           \
  }                                             \
  void inline lock (void)                       \
  {                                             \
    g_rec_mutex_lock (&this->mutex);            \
  }                                             \
  void inline unlock (void)                     \
  {                                             \
    g_rec_mutex_unlock (&this->mutex);          \
  }

// Time.
typedef guint64 GingaTime;    // nanoseconds
typedef gint64 GingaTimeDiff;

#define ginga_gettime()        ((GingaTime)(g_get_monotonic_time () * 1000))
#define GINGA_TIME_NONE        ((GingaTime) -1)
#define GINGA_TIME_IS_VALID(t) (((GingaTime)(t)) != GINGA_TIME_NONE)
#define GINGA_STIME_NONE       ((GingaTimeDiff)G_MININT64)
#define GINGA_STIME_IS_VALID(t) (((GingaTimeDiff)(t)) != GINGA_STIME_NONE)

#define GINGA_SECOND\
  ((GingaTimeDiff)(G_USEC_PER_SEC * G_GINT64_CONSTANT (1000)))

#define GINGA_MSECOND\
  ((GingaTimeDiff)(GINGA_SECOND / G_GINT64_CONSTANT (1000)))

#define GINGA_USECOND\
  ((GingaTimeDiff)(GINGA_SECOND / G_GINT64_CONSTANT (1000000)))

#define GINGA_NSECOND\
  ((GingaTimeDiff)(GINGA_SECOND / G_GINT64_CONSTANT (1000000000)))

#define GINGA_TIME_AS_SECONDS(t)  ((t) / GINGA_SECOND)
#define GINGA_TIME_AS_MSECONDS(t) ((t) / G_GINT64_CONSTANT (1000000))
#define GINGA_TIME_AS_USECONDS(t) ((t) / G_GINT64_CONSTANT (1000))
#define GINGA_TIME_AS_NSECONDS(t) (t)
#define GINGA_DIFF(s, e)          (GingaTimeDiff)((e) - (s))

#define GINGA_TIME_FORMAT "u:%02u:%02u.%09u"
#define GINGA_TIME_ARGS(t)                                              \
  GINGA_TIME_IS_VALID (t) ?                                             \
  (guint) (((GingaTime)(t)) / (GINGA_SECOND * 60 * 60)) : 99,           \
    GINGA_TIME_IS_VALID (t) ?                                           \
    (guint) ((((GingaTime)(t)) / (GINGA_SECOND * 60)) % 60) : 99,       \
    GINGA_TIME_IS_VALID (t) ?                                           \
    (guint) ((((GingaTime)(t)) / GINGA_SECOND) % 60) : 99,              \
    GINGA_TIME_IS_VALID (t) ?                                           \
    (guint) (((GingaTime)(t)) % GINGA_SECOND) : 999999999

#define GINGA_STIME_FORMAT "c%" GINGA_TIME_FORMAT
#define GINGA_STIME_ARGS(t)                                             \
  ((t) == GINGA_STIME_NONE || (t) >= 0) ? '+' : '-',                    \
    GINGA_STIME_IS_VALID (t) ?                                          \
    (guint) (((GingaTime)(ABS(t))) / (GINGA_SECOND * 60 * 60)) : 99,    \
    GINGA_STIME_IS_VALID (t) ?                                          \
    (guint) ((((GingaTime)(ABS(t))) / (GINGA_SECOND * 60)) % 60) : 99,  \
    GINGA_STIME_IS_VALID (t) ?                                          \
    (guint) ((((GingaTime)(ABS(t))) / GINGA_SECOND) % 60) : 99,         \
    GINGA_STIME_IS_VALID (t) ?                                          \
    (guint) (((GingaTime)(ABS(t))) % GINGA_SECOND) : 999999999

// Conversion tables.
bool ginga_color_parse (const string &, SDL_Color *);
bool ginga_color_table_index (const string &, SDL_Color *);
bool ginga_key_table_index (SDL_Keycode, string *);
bool ginga_mime_table_index (string, string *);

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
int xstrtopixel (const string &, int);

bool _xstrtotime (const string &, GingaTime *);
GingaTime xstrtotime (const string &);

int xstrcasecmp (const string &, const string &);
#define xstrcaseeq(s1, s2) (xstrcasecmp ((s1), (s2)) == 0)
int G_GNUC_PRINTF (2,3) xstrassign (string &, const char *, ...);
string G_GNUC_PRINTF (1,2) xstrbuild (const char *, ...);
string xstrup (string);
string xstrdown (string);
string xstrchomp (string);
void xstrreplaceall (string &, const string &, const string &);
vector<string> xstrsplit (const string &, char);

// Path functions.
string xpathbasename (string);
string xpathdirname (string);
bool xpathisabs (const string &);
bool xpathisuri (const string &);
string xpathmakeabs (string);
string xpathbuild (const string &, const string &);
string xpathbuildabs (const string &, const string &);

#endif /* GINGA_H */
