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
#include "Media.h"

const char *LuaAPI::MEDIA = "Ginga.Media";

static const struct luaL_Reg _Media_methods[] =
{
 {"__tostring", LuaAPI::__l_Object_toString},
 {"__getUnderlyingObject", LuaAPI::__l_Object_getUnderlyingObject},
 {"getType", LuaAPI::l_Object_getType},
 {"getDocument", LuaAPI::l_Object_getDocument},
 {"getParent", LuaAPI::l_Object_getParent},
 {"getId", LuaAPI::l_Object_getId},
 {"setProperty", LuaAPI::l_Object_setProperty},
 {NULL, NULL},
};

void
LuaAPI::_Media_attachWrapper (lua_State *L, Media *media)
{
  Media **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (media != NULL);

  LuaAPI::loadLuaWrapperMt (L, _Media_methods, LuaAPI::MEDIA, NULL, 0);

  wrapper = (Media **) lua_newuserdata (L, sizeof (Media **));
  g_assert_nonnull (wrapper);
  *wrapper = media;
  luaL_setmetatable (L, LuaAPI::MEDIA);

  // Set media:=wrapper in LUA_REGISTY.
  LuaAPI::attachLuaWrapper (L, media);

  // Set _D.object[id]:=wrapper, where id is the id of media.
  LuaAPI::_Object_attachWrapper_tail (L, media);
}

void
LuaAPI::_Media_detachWrapper (lua_State *L, Media *media)
{
  LuaAPI::_Object_detachWrapper (L, media);
}

Media *
LuaAPI::_Media_check (lua_State *L, int i)
{
  return *((Media **) luaL_checkudata (L, i, LuaAPI::MEDIA));
}
