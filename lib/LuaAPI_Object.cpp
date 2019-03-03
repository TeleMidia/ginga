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
#include "Object.h"

#include "Context.h"
#include "Media.h"
#include "Switch.h"

const char *const LuaAPI::_Object_optTypes[] =
  {"context", "switch", "media", NULL};

Object::Type
LuaAPI::_Object_getOptIndexType (int i)
{
  switch (i)
    {
    case 0:
      return Object::CONTEXT;
    case 1:
      return Object::SWITCH;
    case 2:
      return Object::MEDIA;
    default:
      g_assert_not_reached ();
    }
}

const char *
LuaAPI::_Object_getRegistryKey (Object *obj)
{
  g_return_val_if_fail (obj != NULL, NULL);

  switch (obj->getType ())
    {
    case Object::CONTEXT:
      return LuaAPI::CONTEXT;
    case Object::SWITCH:
      return LuaAPI::SWITCH;
    case Object::MEDIA:
    case Object::MEDIA_SETTINGS:
      return LuaAPI::MEDIA;
    default:
      g_assert_not_reached ();
    }
}

void
LuaAPI::_Object_attachWrapper (lua_State *L, Object *obj)
{
  static const struct luaL_Reg funcs[] =
    {
     {"__tostring",            LuaAPI::__l_Object_toString},
     {"__getUnderlyingObject", LuaAPI::__l_Object_getUnderlyingObject},
     {"getType",               LuaAPI::l_Object_getType},
     {"getDocument",           LuaAPI::l_Object_getDocument},
     {"getParent",             LuaAPI::l_Object_getParent},
     {"getId",                 LuaAPI::l_Object_getId},
     {"getProperty",           LuaAPI::l_Object_getProperty},
     {"setProperty",           LuaAPI::l_Object_setProperty},
     {"getEvents",             LuaAPI::l_Object_getEvents},
     {"getEventById",          LuaAPI::l_Object_getEventById},
     {NULL, NULL},
    };
  const char *name;

  g_return_if_fail (L != NULL);
  g_return_if_fail (obj != NULL);

  // Load and initialize metatable, if not loaded yet.
  name = LuaAPI::_Object_getRegistryKey (obj);
  LuaAPI::loadLuaWrapperMt (L, funcs, name,
                            (const char *) LuaAPI::Object_initMt_lua,
                            (size_t) LuaAPI::Object_initMt_lua_len);

  // Create Lua wrapper for object.
  switch (obj->getType ())
    {
    case Object::CONTEXT:
      {
        Context **wrapper;
        wrapper = (Context **) lua_newuserdata (L, sizeof (Context **));
        g_assert_nonnull (wrapper);
        *wrapper = (Context *) obj;
        g_assert_nonnull (*wrapper);
        break;
      }
    case Object::SWITCH:
      {
        Switch **wrapper;
        wrapper = (Switch **) lua_newuserdata (L, sizeof (Switch **));
        g_assert_nonnull (wrapper);
        *wrapper = (Switch *) obj;
        g_assert_nonnull (*wrapper);
        break;
      }
    case Object::MEDIA:
    case Object::MEDIA_SETTINGS:
      {
        Media **wrapper = (Media **) lua_newuserdata (L, sizeof (Media **));
        g_assert_nonnull (wrapper);
        *wrapper = (Media *) obj;
        g_assert_nonnull (*wrapper);
        break;
      }
    default:
      g_assert_not_reached ();
    }

  luaL_setmetatable (L, name);

  // Set LUA_REGISTRY[obj]=wrapper.
  LuaAPI::attachLuaWrapper (L, obj);

  // Call obj:__attachData().
  LuaAPI::callLuaWrapper (L, obj, "_attachData", 0, 0);

  // Call _D:_addObject (obj).
  LuaAPI::pushLuaWrapper (L, obj);
  LuaAPI::callLuaWrapper (L, obj->getDocument (), "_addObject", 1, 0);
}

void
LuaAPI::_Object_detachWrapper (lua_State *L, Object *obj)
{
  g_return_if_fail (L != NULL);
  g_return_if_fail (obj != NULL);

  // Call obj:__detachData().
  LuaAPI::callLuaWrapper (L, obj, "_detachData", 0, 0);

  // Call _D:_removeObject (obj).
  LuaAPI::pushLuaWrapper (L, obj);
  LuaAPI::callLuaWrapper (L, obj->getDocument (), "_removeObject", 1, 0);

  // Set LUA_REGISTRY[obj] = nil.
  LuaAPI::detachLuaWrapper (L, obj);
}

Object *
LuaAPI::_Object_check (lua_State *L, int i)
{
  const char *name;

  luaL_checktype (L, i, LUA_TUSERDATA);
  if (unlikely (!lua_getmetatable (L, i)))
    {
      goto fail;
    }

  lua_getfield (L, -1, "__name");
  name = lua_tostring (L, -1);
  if (unlikely (name == NULL))
    {
      lua_pop (L, 1);
      goto fail;
    }

  lua_pop (L, 2);
  if (g_str_equal (name, LuaAPI::CONTEXT))
    return LuaAPI::_Context_check (L, i);
  else if (g_str_equal (name, LuaAPI::SWITCH))
    return LuaAPI::_Switch_check (L, i);
  else if (g_str_equal (name, LuaAPI::MEDIA))
    return LuaAPI::_Media_check (L, i);

 fail:
  luaL_argcheck (L, 0, i, "Ginga.Object expected");
  return NULL;
}

int
LuaAPI::__l_Object_toString (lua_State *L)
{
  Object *obj;

  obj = LuaAPI::_Object_check (L, 1);
  lua_pushstring (L, obj->toString ().c_str ());

  return 1;
}

int
LuaAPI::__l_Object_getUnderlyingObject (lua_State *L)
{
  lua_pushlightuserdata (L, LuaAPI::_Object_check (L, 1));
  return 1;
}

int
LuaAPI::l_Object_getType (lua_State *L)
{
  Object *obj;
  const char *type;

  obj = LuaAPI::_Object_check (L, 1);
  type = Object::getTypeAsString (obj->getType ()).c_str ();
  lua_pushstring (L, type);

  return 1;
}

int
LuaAPI::l_Object_getDocument (lua_State *L)
{
  Object *obj;

  obj = LuaAPI::_Object_check (L, 1);
  LuaAPI::pushLuaWrapper (L, obj->getDocument ());

  return 1;
}

int
LuaAPI::l_Object_getParent (lua_State *L)
{
  Object *obj;

  obj = LuaAPI::_Object_check (L, 1);
  if (obj->getParent () != NULL)
    LuaAPI::pushLuaWrapper (L, obj->getParent ());
  else
    lua_pushnil (L);

  return 1;
}

int
LuaAPI::l_Object_getId (lua_State *L)
{
  Object *obj;

  obj = LuaAPI::_Object_check (L, 1);
  lua_pushstring (L, obj->getId ().c_str ());

  return 1;
}

int
LuaAPI::l_Object_getProperty (lua_State *L)
{
  Object *obj;
  const char *name;

  obj = LuaAPI::_Object_check (L, 1);
  name = luaL_checkstring (L, 2);

  lua_pushstring (L, obj->getProperty (name).c_str ());

  return 1;
}

int
LuaAPI::l_Object_setProperty (lua_State *L)
{
  Object *obj;
  const gchar *name;
  const gchar *value;
  lua_Integer dur;

  obj = LuaAPI::_Object_check (L, 1);
  name = luaL_checkstring (L, 2);
  value = luaL_checkstring (L, 3);
  dur = luaL_optinteger (L, 4, 0);

  obj->setProperty (name, value, dur);

  return 0;
}

int
LuaAPI::l_Object_getEvents (lua_State *L)
{
  Object *obj;
  set<Event *> events;
  lua_Integer i;

  obj = LuaAPI::_Object_check (L, 1);
  obj->getEvents (&events);

  lua_newtable (L);
  i = 1;
  for (auto evt: events)
    {
      LuaAPI::pushLuaWrapper (L, evt);
      lua_rawseti (L, -2, i++);
    }

  return 1;
}

int
LuaAPI::l_Object_getEventById (lua_State *L)
{
  Object *obj;
  int idx;
  const char *id;

  Event::Type type;
  Event *evt;

  obj = LuaAPI::_Object_check (L, 1);
  idx = luaL_checkoption (L, 2, NULL, LuaAPI::_Event_optTypes);
  id = luaL_checkstring (L, 3);

  type = LuaAPI::_Event_getOptIndexType (idx);
  evt = obj->getEventById (type, id);
  if (evt == NULL)
    {
      lua_pushnil (L);
      return 1;
    }

  LuaAPI::pushLuaWrapper (L, evt);
  return 1;
}
