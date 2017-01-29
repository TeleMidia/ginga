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

#if G_GNUC_CHECK_VERSION(4,2)
# define _GCC_PRAGMA(x) _Pragma (G_STRINGIFY (x))
# define GINGA_PRAGMA_DIAG(x) _GCC_PRAGMA (GCC diagnostic x)
#elif defined (__clang__)
# define _CLANG_PRAGMA(x) _Pragma (G_STRINGIFY (x))
# define GINGA_PRAGMA_DIAG(x) _CLANG_PRAGMA (clang diagnostic x)
#else
# define GINGA_PRAGMA_DIAG(x)
#endif

#if G_GNUC_CHECK_VERSION(4,6) || defined (__clang__)
# define GINGA_PRAGMA_DIAG_PUSH()    GINGA_PRAGMA_DIAG (push)
# define GINGA_PRAGMA_DIAG_POP()     GINGA_PRAGMA_DIAG (pop)
#else
# define GINGA_PRAGMA_DIAG_PUSH()
# define GINGA_PRAGMA_DIAG_POP()
#endif
#define GINGA_PRAGMA_DIAG_IGNORE(x)\
  GINGA_PRAGMA_DIAG (ignored G_STRINGIFY (x))
#define GINGA_PRAGMA_DIAG_WARNING(x)\
  GINGA_PRAGMA_DIAG (warning G_STRINGIFY (x))

#include <pthread.h>
#include <curl/curl.h>
#include <ncluaw.h>

GINGA_PRAGMA_DIAG_PUSH ()
GINGA_PRAGMA_DIAG_IGNORE (-Wconversion)
GINGA_PRAGMA_DIAG_IGNORE (-Wswitch-default)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_syswm.h>
GINGA_PRAGMA_DIAG_POP ()

#if WITH_ESPEAK
# include <espeak/speak_lib.h>
#endif

#if WITH_ISDBT
# include <expat.h>
# if WITH_LINUXDVB
#  include <linux/dvb/version.h>
#  include <linux/dvb/frontend.h>
#  include <linux/dvb/dmx.h>
# endif
#endif

#if WITH_MULTIDEVICE
# include <jerror.h>
# include <jpeglib.h>
#endif

#if WITH_LIBRSVG
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

// External C++ libraries.
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/util/XercesDefs.hpp>
XERCES_CPP_NAMESPACE_USE

// Namespaces.
#define _GINGA_NS_BEGIN(t)      namespace t {/*}*/
#define _GINGA_NS_END                     /*{*/}
#define _GINGA_BEGIN(t)       _GINGA_NS_BEGIN (ginga) _GINGA_NS_BEGIN (t)
#define _GINGA_END            _GINGA_NS_END _GINGA_NS_END
#define GINGA_CTXMGMT_BEGIN   _GINGA_BEGIN (ctxmgmt)
#define GINGA_CTXMGMT_END     _GINGA_END
#define GINGA_DATAPROC_BEGIN  _GINGA_BEGIN (dataproc)
#define GINGA_DATAPROC_END    _GINGA_END
#define GINGA_FORMATTER_BEGIN _GINGA_BEGIN (formatter)
#define GINGA_FORMATTER_END   _GINGA_END
#define GINGA_IC_BEGIN        _GINGA_BEGIN (ic)
#define GINGA_IC_END          _GINGA_END
#define GINGA_LSSM_BEGIN      _GINGA_BEGIN (lssm)
#define GINGA_LSSM_END        _GINGA_END
#define GINGA_MB_BEGIN        _GINGA_BEGIN (mb)
#define GINGA_MB_END          _GINGA_END
#define GINGA_MULTIDEV_BEGIN  _GINGA_BEGIN (multidev)
#define GINGA_MULTIDEV_END    _GINGA_END
#define GINGA_NCL_BEGIN       _GINGA_BEGIN (ncl)
#define GINGA_NCL_END         _GINGA_END
#define GINGA_NCLCONV_BEGIN   _GINGA_BEGIN (nclconv)
#define GINGA_NCLCONV_END     _GINGA_END
#define GINGA_PLAYER_BEGIN    _GINGA_BEGIN (player)
#define GINGA_PLAYER_END      _GINGA_END
#define GINGA_SYSTEM_BEGIN    _GINGA_BEGIN (system)
#define GINGA_SYSTEM_END      _GINGA_END
#define GINGA_TSPARSER_BEGIN  _GINGA_BEGIN (tsparser)
#define GINGA_TSPARSER_END    _GINGA_END
#define GINGA_TUNER_BEGIN     _GINGA_BEGIN (tuner)
#define GINGA_TUNER_END       _GINGA_END
#define GINGA_UTIL_BEGIN      _GINGA_BEGIN (util)
#define GINGA_UTIL_END        _GINGA_END

// Macros.
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


// Auxiliary number functions.

// Tests whether two floating-point numbers are equal.
static bool G_GNUC_UNUSED
xnumeq (double x, double y, double epsilon=.0000001)
{
  return ABS (x - y) <= epsilon;
}


// Auxiliary string functions.

// Converts string to double.
static inline bool
_xstrtod (const string &s, double *dp)
{
  const gchar *c_str;
  gchar *endptr;
  double d;

  c_str = s.c_str ();
  d = g_ascii_strtod (c_str, &endptr);
  if (endptr == c_str)
    return false;

  set_if_nonnull (dp, d);
  return true;
}

// Converts string to gint64.
static inline bool
_xstrtoll (const string &s, gint64 *ip, guint base=10)
{
  const gchar *c_str;
  gchar *endptr;
  gint64 i;

  c_str = s.c_str ();
  i = g_ascii_strtoll (c_str, &endptr, base);
  if (endptr == c_str)
    return false;

  set_if_nonnull (ip, i);
  return true;
}

// Converts string to guint64.
static inline bool
_xstrtoull (const string &s, guint64 *ip, guint base=10)
{
  const gchar *c_str;
  gchar *endptr;
  guint64 u;

  c_str = s.c_str ();
  u = g_ascii_strtoull (c_str, &endptr, base);
  if (endptr == c_str)
    return false;

  set_if_nonnull (ip, u);
  return true;
}

// Asserted wrappers for _xstrtod, _xstrtoll, and _xstrtoull.
static inline double
xstrtod (const string &s)
{
  double d;
  g_assert (_xstrtod (s, &d));
  return d;
}

#define _GINGA_XSTRTO_DEFN(Type, Typemin, Typemax)      \
  static inline g##Type                                 \
  xstrto_##Type (const string &s, guint8 base=10)       \
  {                                                     \
    gint64 x;                                           \
    g_assert (_xstrtoll (s, &x, base));                 \
    return (g##Type)(CLAMP (x, Typemin, Typemax));      \
  }

_GINGA_XSTRTO_DEFN  (int,    G_MININT,    G_MAXINT)
_GINGA_XSTRTO_DEFN  (int8,   G_MININT8,   G_MAXINT8)
_GINGA_XSTRTO_DEFN  (int64,  G_MININT64,  G_MAXINT64)

#define _GINGA_XSTRTOU_DEFN(Type, Typemax)              \
  static inline g##Type                                 \
  xstrto_##Type (const string &s, guint8 base=10)       \
  {                                                     \
    guint64 x;                                          \
    g_assert (_xstrtoull (s, &x, base));                \
    return (g##Type)(MIN (x, Typemax));                 \
  }

_GINGA_XSTRTOU_DEFN (uint,   G_MAXUINT)
_GINGA_XSTRTOU_DEFN (uint8,  G_MAXUINT8)
_GINGA_XSTRTOU_DEFN (uint64, G_MAXUINT64)

// Compares two strings ignoring case.
static inline int
xstrcasecmp (string s1, string s2)
{
  return g_ascii_strcasecmp (s1.c_str (), s2.c_str ());
}

// Tests whether two strings are equal ignoring case.
#define xstrcaseeq(s1, s2) (xstrcasecmp ((s1), (s2)) == 0)

// Assigns format to string.
static inline int G_GNUC_PRINTF (2,3)
xstrassign (string &s, const char *format, ...)
{
  va_list args;
  char *c_str = NULL;
  int n;

  va_start (args, format);
  n = g_vasprintf (&c_str, format, args);
  va_end (args);

  g_assert (n >= 0);
  g_assert_nonnull (c_str);
  s.assign (c_str);
  g_free (c_str);

  return n;
}

// Builds string from format.
static inline string G_GNUC_PRINTF (1,2)
xstrbuild (const char *format, ...)
{
  va_list args;
  char *c_str = NULL;
  int n;
  string s;

  va_start (args, format);
  n = g_vasprintf (&c_str, format, args);
  va_end (args);

  g_assert (n >= 0);
  g_assert_nonnull (c_str);
  s.assign (c_str);
  g_free (c_str);

  return s;
}

// Converts string to uppercase.
static inline string
xstrup (string s)
{
  gchar *dup = g_ascii_strup (s.c_str (), s.size ());
  s.assign (dup);
  free (dup);
  return s;
}

// Converts string to lowercase.
static inline string
xstrdown (string s)
{
  gchar *dup = g_ascii_strdown (s.c_str (), s.size ());
  s.assign (dup);
  free (dup);
  return s;
}

// Removes leading and trailing whitespace from string.
static inline string
xstrchomp (string s)
{
  gchar *dup = g_strdup (s.c_str ());
  g_strchomp (dup);
  s.assign (dup);
  g_free (dup);
  return s;
}


// Auxiliary system functions.

// Returns the running time in microseconds.
static inline gint64
xruntime ()
{
  static gint64 t0 = -1;
  if (unlikely (t0 < 0))
      t0 = g_get_monotonic_time ();
  return g_get_monotonic_time () - t0;
}

// Returns the running time in milliseconds.
// FIXME: Use gint64 and time in microseconds.
#define xruntime_ms() (double)(xruntime () * 1000)

#endif /* GINGA_H */
