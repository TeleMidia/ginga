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

#ifndef COMMON_H
#define COMMON_H

#ifdef  __cplusplus
# define GINGA_BEGIN_DECLS extern "C" { /* } */
# define GINGA_END_DECLS           /* { */ }
#else
# define GINGA_BEGIN_DECLS
# define GINGA_END_DECLS
#endif

GINGA_BEGIN_DECLS

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <ncluaw.h>
#include <SDL.h>

#define gpointerof(p) ((gpointer)((ptrdiff_t)(p)))

GINGA_END_DECLS

#include <exception>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/util/XercesDefs.hpp>
XERCES_CPP_NAMESPACE_USE

#endif /* COMMON_H */
