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
GINGA_PRAGMA_DIAG_IGNORE (-Wswitch-default)
#include <SDL.h>
#include <SDL_bits.h>
#include <SDL_endian.h>
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

#define gpointerof(p) ((gpointer)((ptrdiff_t)(p)))

GINGA_END_DECLS

// C++ library.
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

// Typedefs.
typedef void *UnderlyingWindowID;
typedef unsigned long GingaWindowID;
typedef short GingaScreenID;
typedef unsigned int GingaSurfaceID;
typedef unsigned int GingaProviderID;

// Macros.
#define GINGA_ASSERT_GLOBAL_NONNULL(G, Type)                    \
  ((G != NULL) ? (G)                                            \
   : (g_log (G_LOG_DOMAIN, G_LOG_LEVEL_CRITICAL,                \
             "global variable %s is null", G_STRINGIFY (G)),    \
      ((Type) NULL)))

#define streq(a,b) (g_strcmp0 ((a),(b)) == 0)

#endif /* GINGA_H */
