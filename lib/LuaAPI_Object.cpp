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
#include "Context.h"
#include "Media.h"
#include "Object.h"

const char *const LuaAPI::_Object_optTypes[] =
  {"context", "switch", "media", NULL};

Object::Type
LuaAPI::_Object_getTypeFromOptIndex (int i)
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

void
LuaAPI::_Object_attachWrapper_tail (lua_State *L, Object *obj)
{
  LuaAPI::pushLuaWrapper (L, obj);
  LuaAPI::callLuaWrapper (L, obj->getDocument (), "_addObject", 1, 0);
}

void
LuaAPI::_Object_detachWrapper (lua_State *L, Object *obj)
{
  LuaAPI::pushLuaWrapper (L, obj);
  LuaAPI::callLuaWrapper (L, obj->getDocument (), "_removeObject", 1, 0);

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
