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

// This file is included by Document.cpp.

#include "LuaAPI.h"

static int
__l_document_gc (unused (lua_State *L))
{
  // The wrapped Document should not be deleted by Lua's GC.
  return 0;
}

static int
__l_document_toString (lua_State *L)
{
  Document *doc;

  doc = CHECK_DOCUMENT (L, 1);
  lua_pushstring (L, doc->toString ().c_str ());

  return 1;
}

static int
__l_document_getUnderlyingObject (lua_State *L)
{
  lua_pushlightuserdata (L, CHECK_DOCUMENT (L, 1));
  return 1;
}

static int
l_document_getObjectById (lua_State *L)
{
  Document *doc;
  const gchar *id;
  Object *obj;

  doc = CHECK_DOCUMENT (L, 1);
  id = luaL_checkstring (L, 2);

  obj = doc->getObjectById (string (id));
  if (obj == NULL)
    {
      lua_pushnil (L);
      return 1;
    }

  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_rawgetp (L, -1, obj);
  return 1;
}

static const struct luaL_Reg funcs[] =
{
 {"__gc", __l_document_gc},
 {"__tostring", __l_document_toString},
 {"__getUnderlyingObject", __l_document_getUnderlyingObject},
 {"getObjectById", l_document_getObjectById},
 {NULL, NULL},
};

static void
document_attach_lua_api (lua_State *L, Document *doc)
{
  Document **wrapper;

  // Load metatable if necessary.
  luaL_getmetatable (L, GINGA_LUA_API_DOCUMENT);
  if (lua_isnil (L, -1))
    {
      lua_pop (L, 1);
      luax_newmetatable (L, GINGA_LUA_API_DOCUMENT);
      luaL_setfuncs (L, funcs, 0);
    }
  lua_pop (L, 1);

  wrapper = (Document **) lua_newuserdata (L, sizeof (Document **));
  g_assert_nonnull (wrapper);
  *wrapper = doc;
  luaL_setmetatable (L, GINGA_LUA_API_DOCUMENT);

  // Set Document:=Wrapper in LUA_REGISTY.
  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_pushvalue (L, -2);
  lua_rawsetp (L, -2, doc);
  lua_pop (L, 1);

  // Set L:=Document in LUA_REGISTRY.
  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata (L, L);
  lua_pushvalue (L, -3);
  lua_rawset (L, -3);
  lua_pop (L, 1);

  // Set _D:=Document.
  lua_setglobal (L, "_D");
}
