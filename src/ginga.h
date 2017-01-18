/* Copyright (C) 2007-2017 PUC-Rio/Laboratorio TeleMidia

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
# define GINGA_BEGIN_DECLS extern "C" { /* } */
# define GINGA_END_DECLS           /* { */ }
#else
# define GINGA_BEGIN_DECLS
# define GINGA_END_DECLS
#endif

GINGA_BEGIN_DECLS

#if HAVE_CONFIG_H
# include "config.h"
#endif

// C library.
#include <assert.h>
#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

// External C libraries.
#include <pthread.h>
#include <curl/curl.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <ncluaw.h>
#include <SDL.h>
#include <SDL_bits.h>

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

// Typedefs.
typedef void *UnderlyingWindowID;
typedef unsigned long GingaWindowID;
typedef short GingaScreenID;
typedef unsigned int GingaSurfaceID;
typedef unsigned int GingaProviderID;

// Namespaces.
#define NS_BEGIN(t)\
  namespace t {

#define NS_BEGIN2(t1, t2)\
  NS_BEGIN (t1) NS_BEGIN (t2)

#define NS_BEGIN3(t1, t2, t3)\
  NS_BEGIN2 (t1, t2) NS_BEGIN (t3)

#define NS_BEGIN4(t1, t2, t3, t4)\
  NS_BEGIN3 (t1, t2, t3) NS_BEGIN(t4)

#define NS_BEGIN5(t1, t2, t3, t4, t5)\
  NS_BEGIN4 (t1, t2, t3, t4) NS_BEGIN (t5)

#define NS_BEGIN6(t1, t2, t3, t4, t5, t6)\
  NS_BEGIN5 (t1, t2, t3, t4, t5) NS_BEGIN (t6)

#define NS_BEGIN7(t1, t2, t3, t4, t5, t6, t7)\
  NS_BEGIN6 (t1, t2, t3, t4, t5, t6) NS_BEGIN (t7)

#define NS_BEGIN8(t1, t2, t3, t4, t5, t6, t7, t8)\
  NS_BEGIN7 (t1, t2, t3, t4, t5, t6, t7) NS_BEGIN (t8)

#define NS_END  }
#define NS_END2 }}
#define NS_END3 }}}
#define NS_END4 }}}}
#define NS_END5 }}}}}
#define NS_END6 }}}}}}
#define NS_END7 }}}}}}}
#define NS_END8 }}}}}}}}

#define GINGA_BEGIN             NS_BEGIN  (ginga)
#define GINGA_END               NS_END
#define GINGA_CTXMGMT_BEGIN     NS_BEGIN2 (ginga, ctxmgmt)
#define GINGA_CTXMGMT_END       NS_END2
#define GINGA_DATAPROC_BEGIN    NS_BEGIN2 (ginga, dataproc)
#define GINGA_DATAPROC_END      NS_END2
#define GINGA_FORMATTER_BEGIN   NS_BEGIN2 (ginga, formatter)
#define GINGA_FORMATTER_END     NS_END2
#define GINGA_IC_BEGIN          NS_BEGIN2 (ginga, ic)
#define GINGA_IC_END            NS_END2
#define GINGA_LSSM_BEGIN        NS_BEGIN2 (ginga, lssm)
#define GINGA_LSSM_END          NS_END2
#define GINGA_MB_BEGIN          NS_BEGIN2 (ginga, mb)
#define GINGA_MB_END            NS_END2
#define GINGA_MULTIDEVICE_BEGIN NS_BEGIN2 (ginga, multidevice)
#define GINGA_MULTIDEVICE_END   NS_END2
#define GINGA_NCL_BEGIN         NS_BEGIN2 (ginga, ncl)
#define GINGA_NCL_END           NS_END2
#define GINGA_NCLCONV_BEGIN     NS_BEGIN2 (ginga, nclconv)
#define GINGA_NCLCONV_END       NS_END2
#define GINGA_PLAYER_BEGIN      NS_BEGIN2 (ginga, player)
#define GINGA_PLAYER_END        NS_END2
#define GINGA_SYSTEM_BEGIN      NS_BEGIN2 (ginga, system)
#define GINGA_SYSTEM_END        NS_END2
#define GINGA_TSPARSER_BEGIN    NS_BEGIN2 (ginga, tsparser)
#define GINGA_TSPARSER_END      NS_END2
#define GINGA_TUNER_BEGIN       NS_BEGIN2 (ginga, tuner)
#define GINGA_TUNER_END         NS_END2
#define GINGA_UTIL_BEGIN        NS_BEGIN2 (ginga, util)
#define GINGA_UTIL_END          NS_END2

#endif /* GINGA_H */
