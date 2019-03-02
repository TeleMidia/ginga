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

#include "LuaAPI.h"
#include "Document.h"
#include "Media.h"

const char *LuaAPI::DOCUMENT = "Ginga.Document";

static const struct luaL_Reg _Document_methods[] =
{
 {"__tostring", LuaAPI::__l_Document_toString},
 {"__getUnderlyingObject", LuaAPI::__l_Document_getUnderlyingObject},
 {"getObjectById", LuaAPI::l_Document_getObjectById},
 {"getMediaObjects", LuaAPI::l_Document_getMediaObjects},
 {NULL, NULL},
};

void
LuaAPI::_Document_attachWrapper (lua_State *L, Document *doc)
{
  Document **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  // Load metatable if necessary.
  luaL_getmetatable (L, LuaAPI::DOCUMENT);
  if (lua_isnil (L, -1))
    {
      lua_pop (L, 1);
      luax_newmetatable (L, LuaAPI::DOCUMENT);
      luaL_setfuncs (L, _Document_methods, 0);
    }
  lua_pop (L, 1);

  wrapper = (Document **) lua_newuserdata (L, sizeof (Document **));
  g_assert_nonnull (wrapper);
  *wrapper = doc;
  luaL_setmetatable (L, LuaAPI::DOCUMENT);

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

Document *
LuaAPI::_Document_check (lua_State *L, int i)
{
  return *((Document **) luaL_checkudata (L, i, LuaAPI::DOCUMENT));
}

int
LuaAPI::__l_Document_toString (lua_State *L)
{
  Document *doc;

  doc = LuaAPI::_Document_check (L, 1);
  lua_pushstring (L, doc->toString ().c_str ());

  return 1;
}

int
LuaAPI::__l_Document_getUnderlyingObject (lua_State *L)
{
  lua_pushlightuserdata (L, LuaAPI::_Document_check (L, 1));
  return 1;
}

int
LuaAPI::l_Document_getObjectById (lua_State *L)
{
  Document *doc;
  const gchar *id;
  Object *obj;

  doc = LuaAPI::_Document_check (L, 1);
  id = luaL_checkstring (L, 2);

  obj = doc->getObjectById (string (id));
  if (obj == NULL)
    {
      lua_pushnil (L);
      return 1;
    }

  LuaAPI::pushLuaWrapper (L, obj);
  return 1;
}

int
LuaAPI::l_Document_getMediaObjects (lua_State *L)
{
  Document *doc;
  set<Object *> objects;
  lua_Integer i;

  doc = LuaAPI::_Document_check (L, 1);
  doc->getObjects (Object::MEDIA, &objects);

  lua_newtable (L);
  i = 1;
  for (auto obj: objects)
    {
      Media *media;

      media = cast (Media *, obj);
      g_assert_nonnull (media);
      LuaAPI::pushLuaWrapper (L, media);
      g_assert (media == LuaAPI::_Media_check (L, -1));
      lua_rawseti (L, -2, i++);
    }

  return 1;
}
