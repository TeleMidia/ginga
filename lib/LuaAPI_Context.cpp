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

const char *LuaAPI::CONTEXT = "Ginga.Context";

static const struct luaL_Reg _Context_methods[] =
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
LuaAPI::_Context_attachWrapper (lua_State *L, Context *ctx)
{
  Context **wrapper;

  g_return_if_fail (L != NULL);
  g_return_if_fail (ctx != NULL);

  LuaAPI::loadLuaWrapperMt (L, _Context_methods, LuaAPI::CONTEXT,
                            NULL, 0);

  wrapper = (Context **) lua_newuserdata (L, sizeof (Context **));
  g_assert_nonnull (wrapper);
  *wrapper = ctx;
  luaL_setmetatable (L, LuaAPI::CONTEXT);

  // Set ctx:=wrapper in LUA_REGISTY.
  LuaAPI::attachLuaWrapper (L, ctx);

  // Set _D.object[id]:=wrapper, where id is the id of ctx.
  LuaAPI::_Object_attachWrapper_tail (L, ctx);
}

void
LuaAPI::_Context_detachWrapper (lua_State *L, Context *ctx)
{
  LuaAPI::_Object_detachWrapper (L, ctx);
}

Context *
LuaAPI::_Context_check (lua_State *L, int i)
{
  return *((Context **) luaL_checkudata (L, i, LuaAPI::CONTEXT));
}
