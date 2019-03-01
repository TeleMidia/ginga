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

// This file is included by Media.cpp.

#include "LuaAPI.h"

static int
__l_Media_toString (lua_State *L)
{
  Media *media;

  media = CHECK_MEDIA (L, 1);
  lua_pushstring (L, media->toString ().c_str ());

  return 1;
}

static int
__l_Media_getUnderlyingObject (lua_State *L)
{
  lua_pushlightuserdata (L, CHECK_MEDIA (L, 1));
  return 1;
}

static int
l_Media_getId (lua_State *L)
{
  Media *media;

  media = CHECK_MEDIA (L, 1);
  lua_pushstring (L, media->getId ().c_str ());

  return 1;
}

static int
l_Media_getType (lua_State *L)
{
  Media *media;

  media = CHECK_MEDIA (L, 1);
  lua_pushstring (L, media->getObjectTypeAsString ().c_str ());

  return 1;
}

static int
l_Media_setProperty (lua_State *L)
{
  Media *media;
  const gchar *name;
  const gchar *value;
  lua_Integer dur;

  media = CHECK_MEDIA (L, 1);
  name = luaL_checkstring (L, 2);
  value = luaL_checkstring (L, 3);
  dur = luaL_optinteger (L, 4, 0);

  media->setProperty (name, value, dur);

  return 0;
}

static const struct luaL_Reg funcs[] =
{
 {"__tostring", __l_Media_toString},
 {"__getUnderlyingObject", __l_Media_getUnderlyingObject},
 {"getId", l_Media_getId},
 {"getType", l_Media_getType},
 {"setProperty", l_Media_setProperty},
 {NULL, NULL},
};

static void
attachLuaAPI_Media (lua_State *L, Media *media)
{
  Media **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (media != NULL);

  // Load metatable if necessary.
  luaL_getmetatable (L, GINGA_LUA_API_MEDIA);
  if (lua_isnil (L, -1))
    {
      lua_pop (L, 1);
      luax_newmetatable (L, GINGA_LUA_API_MEDIA);
      luaL_setfuncs (L, funcs, 0);
    }
  lua_pop (L, 1);

  wrapper = (Media **) lua_newuserdata (L, sizeof (Media **));
  g_assert_nonnull (wrapper);
  *wrapper = media;
  luaL_setmetatable (L, GINGA_LUA_API_MEDIA);

  // Set Media:=Wrapper in LUA_REGISTY.
  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_pushvalue (L, -2);
  lua_rawsetp (L, -2, media);

  lua_pop (L, 2);
}
