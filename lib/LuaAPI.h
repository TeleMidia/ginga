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

#ifndef GINGA_LUA_API_H
#define GINGA_LUA_API_H

#include "aux-ginga.h"
GINGA_BEGIN_DECLS
#include "aux-lua.h"
GINGA_END_DECLS

GINGA_NAMESPACE_BEGIN

class Document;
class Media;

class LuaAPI
{
 public:

  /**
   * @brief Pushes onto stack the Lua wrapper of a pointer.
   * @param L Lua State
   * @param ptr The pointer whose wrapper is to be pushed.
   */
  static void pushLuaWrapper (lua_State *L, void *ptr)
  {
    lua_pushvalue (L, LUA_REGISTRYINDEX);
    lua_rawgetp (L, -1, ptr);
    lua_remove (L, -2);
  }

  // Document:

  /**
   * @brief Registry key for the Document metatable.
   */
  static const char *DOCUMENT;

  static void _Document_attachWrapper (lua_State *L, Document *doc);

  static Document *_Document_check (lua_State *L, int i);

  static int __l_Document_toString (lua_State *L);

  static int __l_Document_getUnderlyingObject (lua_State *L);

  static int l_Document_getObjectById (lua_State *L);

  static int l_Document_getMediaObjects (lua_State *L);

  // Media:

  /**
   * @brief Registry key for the Media metatable.
   */
  static const char *MEDIA;

  static void _Media_attachWrapper (lua_State *L, Media *media);

  static Media *_Media_check (lua_State *L, int i);

  static int __l_Media_toString (lua_State *L);

  static int __l_Media_getUnderlyingObject (lua_State *L);

  static int l_Media_getType (lua_State *L);

  static int l_Media_getId (lua_State *L);

  static int l_Media_setProperty (lua_State *L);
};

GINGA_NAMESPACE_END

#endif // GINGA_LUA_API_H
