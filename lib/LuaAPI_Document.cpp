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

const char *LuaAPI::_DOCUMENT = "Ginga.Document";

// Public.

void
LuaAPI::Document_attachWrapper (lua_State *L, Document *doc)
{
  static const struct luaL_Reg funcs[] =
    {
     {"__tostring",            LuaAPI::__l_Document_toString},
     {"__getUnderlyingObject", LuaAPI::__l_Document_getUnderlyingObject},
     {"getObjects",            LuaAPI::_l_Document_getObjects},
     {"getObject",             LuaAPI::_l_Document_getObject},
     {"getRoot",               LuaAPI::_l_Document_getRoot},
     {"getSettings",           LuaAPI::_l_Document_getSettings},
     {"createObject",          LuaAPI::_l_Document_createObject},
     {NULL, NULL},
    };
  Document **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  // Load and initialize metatable, if not loaded yet.
  LuaAPI::_loadLuaWrapperMt (L, funcs, LuaAPI::_DOCUMENT,
                             (const char *) LuaAPI::Document_initMt_lua,
                             (size_t) LuaAPI::Document_initMt_lua_len);

  // Create Lua wrapper for document.
  wrapper = (Document **) lua_newuserdata (L, sizeof (Document **));
  g_assert_nonnull (wrapper);
  *wrapper = doc;
  luaL_setmetatable (L, LuaAPI::_DOCUMENT);

  // Set LUA_REGISTY[doc]=wrapper.
  lua_pushvalue (L, -1);
  LuaAPI::_attachLuaWrapper (L, doc);

  // Set _G._D=wrapper.
  lua_setglobal (L, "_D");

  // Call _D:__attachData().
  LuaAPI::_callLuaWrapper (L, doc, "_attachData", 0, 0);
}

void
LuaAPI::Document_detachWrapper (lua_State *L, Document *doc)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  // Call _D:__detachData().
  LuaAPI::_callLuaWrapper (L, doc, "_detachData", 1, 0);

  // Set _D:=nil.
  lua_pushnil (L);
  lua_setglobal (L, "_D");

  // Set LUA_REGISTY[doc]=nil.
  LuaAPI::_detachLuaWrapper (L, doc);
}

Document *
LuaAPI::Document_check (lua_State *L, int i)
{
  g_return_val_if_fail (L != NULL, NULL);
  return *((Document **) luaL_checkudata (L, i, LuaAPI::_DOCUMENT));
}

void
LuaAPI::Document_call (lua_State *L, Document *doc, const char *name,
                       int nargs, int nresults)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);
  g_return_if_fail (name != NULL);
  g_return_if_fail (nargs >= 0);
  g_return_if_fail (nresults >= 0);

  LuaAPI::_callLuaWrapper (L, doc, name, nargs, nresults);
}

// Private.

int
LuaAPI::__l_Document_toString (lua_State *L)
{
  Document *doc;

  doc = LuaAPI::Document_check (L, 1);
  lua_pushstring (L, doc->toString ().c_str ());

  return 1;
}

int
LuaAPI::__l_Document_getUnderlyingObject (lua_State *L)
{
  lua_pushlightuserdata (L, LuaAPI::Document_check (L, 1));
  return 1;
}

int
LuaAPI::_l_Document_getObjects (lua_State *L)
{
  Document *doc;
  set<Object *> objects;
  lua_Integer i;

  doc = LuaAPI::Document_check (L, 1);
  doc->getObjects (&objects);

  lua_newtable (L);
  i = 1;
  for (auto obj: objects)
    {
      LuaAPI::_pushLuaWrapper (L, obj);
      lua_rawseti (L, -2, i++);
    }

  return 1;
}

int
LuaAPI::_l_Document_getObject (lua_State *L)
{
  Document *doc;
  const gchar *id;
  Object *obj;

  doc = LuaAPI::Document_check (L, 1);
  id = luaL_checkstring (L, 2);

  obj = doc->getObject (string (id));
  if (obj == NULL)
    {
      lua_pushnil (L);
      return 1;
    }

  LuaAPI::_pushLuaWrapper (L, obj);

  return 1;
}

int
LuaAPI::_l_Document_getRoot (lua_State *L)
{
  Document *doc;

  doc = LuaAPI::Document_check (L, 1);
  LuaAPI::_pushLuaWrapper (L, doc->getRoot ());

  return 1;
}

int
LuaAPI::_l_Document_getSettings (lua_State *L)
{
  Document *doc;

  doc = LuaAPI::Document_check (L, 1);
  LuaAPI::_pushLuaWrapper (L, doc->getSettings ());

  return 1;
}

int
LuaAPI::_l_Document_createObject (lua_State *L)
{
  Document *doc;
  int idx;
  const char *id;

  Composition *parent;
  Object::Type type;
  Object *obj;

  doc = LuaAPI::Document_check (L, 1);
  idx = luaL_checkoption (L, 2, NULL, LuaAPI::_Object_optTypes);
  obj = LuaAPI::_Object_check (L, 3);
  luaL_argcheck (L, obj->getType () == Object::CONTEXT
                 || obj->getType () == Object::SWITCH, 3,
                 "Ginga.Composition expected");
  id = luaL_checkstring (L, 4);

  parent = (Composition *) obj;
  type = LuaAPI::_Object_getOptIndexType (idx);
  obj = doc->createObject (type, parent, id);
  if (unlikely (obj == NULL))
    {
      lua_pushnil (L);
      lua_pushliteral (L, "id already in use");
      return 2;
    }

  LuaAPI::_pushLuaWrapper (L, obj);
  return 1;
}
