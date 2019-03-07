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

#include "Composition.h"
#include "Context.h"
#include "Media.h"
#include "Switch.h"

const char *LuaAPI::_DOCUMENT = "Ginga.Document";

const struct luaL_Reg LuaAPI::_Document_funcs[] =
  {
   {"__gc",                 LuaAPI::__l_Document_gc},
   {"_getUnderlyingObject", LuaAPI::_l_Document_getUnderlyingObject},
   {"createObject",         LuaAPI::_l_Document_createObject},
   {"_createEvent",         LuaAPI::_l_Document_createEvent},
   {NULL, NULL},
  };

// Public.

void
LuaAPI::Document_attachWrapper (lua_State *L, Document *doc)
{
  static const struct luaL_Reg *const funcs[] =
    {
     _Document_funcs,
     NULL
    };

  static const Chunk *const chunks[] =
    {
     &LuaAPI::_initMt,
     &LuaAPI::_Document_initMt,
     NULL
    };

  Document **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  // Load and initialize metatable, if not loaded yet.
  LuaAPI::_loadLuaWrapperMt (L, LuaAPI::_DOCUMENT, funcs, chunks);

  // Create Lua wrapper for document.
  wrapper = (Document **) lua_newuserdata (L, sizeof (Document **));
  g_assert_nonnull (wrapper);
  *wrapper = doc;
  luaL_setmetatable (L, LuaAPI::_DOCUMENT);

  // Set LUA_REGISTY[doc]=wrapper.
  LuaAPI::_attachLuaWrapper (L, doc);

  // Call _D:_attachData().
  LuaAPI::_callLuaWrapper (L, doc, "_attachData", 0, 0);
}

void
LuaAPI::Document_detachWrapper (lua_State *L, Document *doc)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  // Call _D:_detachData().
  LuaAPI::_callLuaWrapper (L, doc, "_detachData", 1, 0);

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
LuaAPI::Document_push (lua_State *L, Document *doc)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (doc != NULL);

  LuaAPI::_pushLuaWrapper (L, doc);
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
LuaAPI::__l_Document_gc (lua_State *L)
{
  Document *doc;

  doc = LuaAPI::Document_check (L, 1);
  delete doc;

  return 0;
}

int
LuaAPI::_l_Document_getUnderlyingObject (lua_State *L)
{
  lua_pushlightuserdata (L, LuaAPI::Document_check (L, 1));
  return 1;
}

int
LuaAPI::_l_Document_createObject (lua_State *L)
{
  Document *doc;
  Object::Type type;
  Composition *parent;
  const char *id;

  Object *obj;

  doc = LuaAPI::Document_check (L, 1);
  type = LuaAPI::Object_Type_check (L, 2);
  parent = LuaAPI::Composition_check (L, 3);
  id = luaL_checkstring (L, 4);

  if (parent->getDocument () != doc)
    goto fail;

  if (doc->getObject (id) != NULL)
    goto fail;

  obj = NULL;
  switch (type)
    {
    case Object::MEDIA:
      obj = new Media (doc, parent, id);
      break;
    case Object::CONTEXT:
      obj = new Context (doc, parent, id);
      break;
    case Object::SWITCH:
      obj = new Switch (doc, parent, id);
      break;
    default:
      g_assert_not_reached ();
    }

  LuaAPI::_pushLuaWrapper (L, obj);
  return 1;

 fail:
  lua_pushnil (L);
  return 1;
}

int
LuaAPI::_l_Document_createEvent (lua_State *L)
{
  Document *doc;
  Event::Type type;
  const char *objId;
  const char *evtId;

  Object *obj;
  Event *evt;

  doc = LuaAPI::Document_check (L, 1);
  type = LuaAPI::Event_Type_check (L, 2);
  objId = luaL_checkstring (L, 3);
  evtId = luaL_checkstring (L, 4);

  obj = doc->getObject (objId);
  if (obj == NULL)
    goto fail;

  evt = obj->createEvent (type, evtId);
  if (evt == NULL)
    goto fail;

  LuaAPI::_pushLuaWrapper (L, evt);
  return 1;

 fail:
  lua_pushnil (L);
  return 1;
}
