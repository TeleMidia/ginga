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
#include "MediaSettings.h"
#include "Switch.h"

#include "Player.h"

#if defined WITH_NCLUA && WITH_NCLUA
#include "PlayerLua.h"
#endif
#include "PlayerGStreamer.h"

// Public.

void
LuaAPI::Document_attachWrapper (lua_State *L, Document *doc)
{
  static const struct luaL_Reg _Document_funcs[] =
    {
     {"__gc",          LuaAPI::__l_Document_gc},
     {"_createObject", LuaAPI::_l_Document_createObject},
     {"_createEvent",  LuaAPI::_l_Document_createEvent},
     {"_createPlayer", LuaAPI::_l_Document_createPlayer},
     {"_isinteger",    LuaAPI::_l_Document_isinteger},
     {NULL, NULL},
    };

  static const struct luaL_Reg *const funcs[] =
    {
     _funcs,
     _Document_funcs,
     NULL
    };

  static const Chunk *const chunks[] =
    {
     &LuaAPI::_initMt,
     &LuaAPI::_Document_initMt,
     &LuaAPI::_traceMt,
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
LuaAPI::_l_Document_createObject (lua_State *L)
{
  Document *doc;
  Object::Type type;
  const char *id;

  Object *obj;

  doc = LuaAPI::Document_check (L, 1);
  type = LuaAPI::Object_Type_check (L, 2);
  id = luaL_checkstring (L, 3);

  if (doc->getObject (id) != NULL)
    {
      lua_pushnil (L);
      return 1;
    }

  obj = NULL;
  switch (type)
    {
    case Object::MEDIA:
      if (g_str_equal (id, "__settings__")) // TODO: REMOVE
        obj = new MediaSettings (doc, id);
      else
        obj = new Media (doc, id);
      break;
    case Object::CONTEXT:
      obj = new Context (doc, id);
      break;
    case Object::SWITCH:
      obj = new Switch (doc, id);
      break;
    default:
      g_assert_not_reached ();
    }

  LuaAPI::_pushLuaWrapper (L, obj);
  return 1;
}

int
LuaAPI::_l_Document_createEvent (lua_State *L)
{
  Event::Type type;
  Object *obj;
  const char *evtId;

  LuaAPI::Document_check (L, 1);
  type = LuaAPI::Event_Type_check (L, 2);
  obj = LuaAPI::Object_check (L, 3);
  evtId = luaL_checkstring (L, 4);

  if (obj->getEvent (type, evtId))
    {
      lua_pushnil (L);
    }
  else
    {
      LuaAPI::_pushLuaWrapper (L, new Event (obj, type, evtId));
    }

  return 1;
}

int
LuaAPI::_l_Document_createPlayer (lua_State *L)
{
  Document *doc;
  Media *media;
  const char *name;

  Player *player = NULL;

  doc = LuaAPI::Document_check (L, 1);
  media = LuaAPI::Media_check (L, 2);
  lua_remove (L, 2);
  LuaAPI::Document_call (L, doc, "_getPlayerName", 1, 1);
  name = luaL_optstring (L, -1, NULL);

  if (name != NULL && g_str_equal (name, "PlayerGStreamer"))
    {
      player = new PlayerGStreamer (media);
    }
#if defined WITH_NCLUA && WITH_NCLUA
  else if (name != NULL && g_str_equal (name, "PlayerLua"))
    {
      player = new PlayerLua (media);
    }
#endif
  else
    {
      player = new Player (media);
    }

  g_assert_nonnull (player);
  LuaAPI::Player_push (L, player);

  return 1;
}

int
LuaAPI::_l_Document_isinteger (lua_State *L)
{
  lua_pushboolean (L, lua_isinteger (L, 1));
  return 1;
}
