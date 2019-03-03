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

void
LuaAPI::_Document_attachWrapper (lua_State *L, Document *doc)
{
  static const struct luaL_Reg funcs[] =
    {
     {"__tostring",            LuaAPI::__l_Document_toString},
     {"__getUnderlyingObject", LuaAPI::__l_Document_getUnderlyingObject},
     {"getObjects",            LuaAPI::l_Document_getObjects},
     {"getObjectById",         LuaAPI::l_Document_getObjectById},
     {"getRoot",               LuaAPI::l_Document_getRoot},
     {"getSettingsObject",     LuaAPI::l_Document_getSettingsObject},
     {"createObject",          LuaAPI::l_Document_createObject},
     {NULL, NULL},
    };
  Document **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  // Load and initialize metatable, if not loaded yet.
  LuaAPI::loadLuaWrapperMt (L, funcs, LuaAPI::DOCUMENT,
                            (const char *) LuaAPI::Document_initMt_lua,
                            (size_t) LuaAPI::Document_initMt_lua_len);

  // Create Lua wrapper for document.
  wrapper = (Document **) lua_newuserdata (L, sizeof (Document **));
  g_assert_nonnull (wrapper);
  *wrapper = doc;
  luaL_setmetatable (L, LuaAPI::DOCUMENT);

  // Set LUA_REGISTY[doc]=wrapper.
  lua_pushvalue (L, -1);
  LuaAPI::attachLuaWrapper (L, doc);

  // Set _G._D=wrapper.
  lua_setglobal (L, "_D");

  // Call _D:__attachData().
  LuaAPI::callLuaWrapper (L, doc, "_attachData", 0, 0);
}

void
LuaAPI::_Document_detachWrapper (lua_State *L, Document *doc)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  // Call _D:__detachData().
  LuaAPI::callLuaWrapper (L, doc, "_detachData", 1, 0);

  // Set _D:=nil.
  lua_pushnil (L);
  lua_setglobal (L, "_D");

  // Set LUA_REGISTY[doc]=nil.
  LuaAPI::detachLuaWrapper (L, doc);
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
LuaAPI::l_Document_getObjects (lua_State *L)
{
  Document *doc;
  set<Object *> objects;
  lua_Integer i;

  doc = LuaAPI::_Document_check (L, 1);
  doc->getObjects (&objects);

  lua_newtable (L);
  i = 1;
  for (auto obj: objects)
    {
      LuaAPI::pushLuaWrapper (L, obj);
      lua_rawseti (L, -2, i++);
    }

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
LuaAPI::l_Document_getRoot (lua_State *L)
{
  Document *doc;

  doc = LuaAPI::_Document_check (L, 1);
  LuaAPI::pushLuaWrapper (L, doc->getRoot ());

  return 1;
}

int
LuaAPI::l_Document_getSettingsObject (lua_State *L)
{
  Document *doc;

  doc = LuaAPI::_Document_check (L, 1);
  LuaAPI::pushLuaWrapper (L, doc->getSettingsObject ());

  return 1;
}

int
LuaAPI::l_Document_createObject (lua_State *L)
{
  Document *doc;
  int idx;
  const char *id;
  Composition *parent;
  Object::Type type;
  Object *obj;

  doc = LuaAPI::_Document_check (L, 1);
  idx = luaL_checkoption (L, 2, NULL, LuaAPI::_Object_optTypes);
  obj = LuaAPI::_Object_check (L, 3);
  luaL_argcheck (L, obj->getType () == Object::CONTEXT
                 || obj->getType () == Object::SWITCH, 3,
                 "Ginga.Composition expected");
  parent = (Composition *) obj;
  id = luaL_checkstring (L, 4);

  type = LuaAPI::_Object_getOptIndexType (idx);
  obj = doc->createObject (type, parent, id);
  if (unlikely (obj == NULL))
    {
      lua_pushnil (L);
      lua_pushliteral (L, "id already in use");
      return 2;
    }

  LuaAPI::pushLuaWrapper (L, obj);
  return 1;
}
