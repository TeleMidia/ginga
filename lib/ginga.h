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
#include <cairo.h>
#include <lua.h>
GINGA_END_DECLS

#include <cstdint>
#include <string>

class Ginga
{
public:
  Ginga ();
  virtual ~Ginga () = 0;

  virtual void *getDocument () = 0;

  virtual bool start (const std::string &path, int w, int h,
                      std::string *errmsg) = 0;

  virtual void resize (int width, int height) = 0;

  virtual bool sendKey (const std::string &key, bool press) = 0;

  static Ginga *create ();

  virtual std::string debug_getDocPath () = 0;
};

#endif // GINGA_H
