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
# define GINGA_BEGIN_DECLS extern "C" { /* } */
# define GINGA_END_DECLS           /* { */ }
#else
# define GINGA_BEGIN_DECLS
# define GINGA_END_DECLS
#endif

GINGA_BEGIN_DECLS

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

#define GINGA_BEGIN             NS_BEGIN (ginga)
#define GINGA_END               NS_END
#define GINGA_CTXMGMT_BEGIN     NS_BEGIN2 (ginga, ctxmgmt)
#define GINGA_CTXMGMT_END       NS_END2
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
#define GINGA_UTIL_BEGIN        NS_BEGIN2 (ginga, util)
#define GINGA_UTIL_END          NS_END2


#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, core, dataprocessing)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_CAROUSEL_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, dataprocessing, carousel)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_CAROUSEL_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, core, dataprocessing, dsmcc, npt)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_DSMCC_NPT_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_EPG_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, dataprocessing, epg)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_EPG_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, dataprocessing, ncl)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_DATAPROCESSING_NCL_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_COMPAT_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, system, compat)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_COMPAT_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_FS_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, system, fs)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_FS_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_THREAD_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, system, thread)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_THREAD_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_TIME_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, system, time)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_SYSTEM_TIME_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, core, tsparser)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, core, tsparser, si)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, core, tsparser, si, descriptors)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TSPARSER_SI_DESCRIPTORS_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TUNING_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, core, tuning)

#define BR_PUCRIO_TELEMIDIA_GINGA_CORE_TUNING_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTATION_CONTEXT_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adaptation, context)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTATION_CONTEXT_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adapters, application)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_LUA_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, ncl, adapters, application, lua)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_LUA_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_NCL_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, ncl, adapters, application, ncl)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_NCL_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_XHTML_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, ncl, adapters, application, xhtml)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_APPLICATION_XHTML_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adapters, av)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_TV_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, ncl, adapters, av, tv)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_AV_TV_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, ncl, adapters)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_IMAGE_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adapters, image)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_IMAGE_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_MIRROR_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adapters, mirror)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_MIRROR_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TEXT_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adapters, text)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TEXT_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TIME_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adapters, time)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TIME_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TTS_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, adapters, tts)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ADAPTERS_TTS_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ANIMATION_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, ncl, animation)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_ANIMATION_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_EMCONVERTER_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, ncl, emconverter)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_EMCONVERTER_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_BEGIN\
  NS_BEGIN5 (br, pucrio, telemidia, ginga, ncl)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_END\
  NS_END5

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_FOCUS_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, ncl, focus)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_FOCUS_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_COMPONENTS_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, model, components)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_COMPONENTS_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, model, event)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, ncl, model, event, transition)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_EVENT_TRANSITION_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, model, link)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_LINK_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, model, presentation)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_FOCUS_BEGIN\
  NS_BEGIN8 (br, pucrio, telemidia, ginga, ncl, model, presentation, focus)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_PRESENTATION_FOCUS_END\
  NS_END8

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_SWITCHES_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, model, switches)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_SWITCHES_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_TIME_BEGIN\
  NS_BEGIN7 (br, pucrio, telemidia, ginga, ncl, model, time)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MODEL_TIME_END\
  NS_END7

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MULTIDEVICE_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, ncl, multidevice)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_MULTIDEVICE_END\
  NS_END6

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_PREFETCH_BEGIN\
  NS_BEGIN6 (br, pucrio, telemidia, ginga, ncl, prefetch)

#define BR_PUCRIO_TELEMIDIA_GINGA_NCL_PREFETCH_END\
  NS_END6

#endif /* GINGA_H */
