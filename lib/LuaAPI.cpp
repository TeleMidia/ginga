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

static void
xloadbuffer (lua_State *L, const char *chunk, size_t len, const char *name)
{
  int err;
  err = luaL_loadbuffer (L, chunk, len, name);
  if (unlikely (err != LUA_OK))
    {
      luax_dump_stack (L);
      ERROR ("%s", lua_tostring (L, -1));
    }
}

static void
xpcall (lua_State *L, int nargs, int nresults)
{
  int err;

  err = lua_pcall (L, nargs, nresults, 0);
  if (unlikely (err != LUA_OK))
    {
      luax_dump_stack (L);
      ERROR ("%s", lua_tostring (L, -1));
    }
}

void
LuaAPI::_loadLuaWrapperMt (lua_State *L, const luaL_Reg *funcs,
                           const char *name, const char *chunk, size_t len)
{
  luaL_getmetatable (L, name);
  if (!lua_isnil (L, -1))
    {
      lua_pop (L, 1);
      return;                 // nothing to do
    }
  lua_pop (L, 1);

  luaL_newmetatable (L, name);
  luaL_setfuncs (L, funcs, 0);
  xloadbuffer (L, (const char *) LuaAPI::initMt_lua,
               (size_t) LuaAPI::initMt_lua_len, "Ginga.LuaAPI");
  lua_pushvalue (L, -2);
  xpcall (L, 1, 0);

  if (chunk == NULL)
    {
      lua_pop (L, 1);
      return;                   // nothing else to do
    }

  xloadbuffer (L, chunk, len, name);
  lua_insert (L, -2);
  xpcall (L, 1, 0);
}

void
LuaAPI::_attachLuaWrapper (lua_State *L, void *ptr)
{
  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_insert (L, -2);
  lua_rawsetp (L, -2, ptr);
  lua_pop (L, 1);
}

void
LuaAPI::_detachLuaWrapper (lua_State *L, void *ptr)
{
  lua_pushnil (L);
  LuaAPI::_attachLuaWrapper (L, ptr);
}

void
LuaAPI::_pushLuaWrapper (lua_State *L, void *ptr)
{
  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_rawgetp (L, -1, ptr);
  lua_remove (L, -2);
}

void
LuaAPI::_callLuaWrapper (lua_State *L, void *ptr, const char *name,
                         int nargs, int nresults)
{
  LuaAPI::_pushLuaWrapper (L, ptr);
  g_assert (luaL_getmetafield (L, -1, name) != LUA_TNIL);

  lua_insert (L, (-nargs) -2);
  lua_insert (L, (-nargs) -1);

  if (unlikely (lua_pcall (L, nargs + 1, nresults, 0) != LUA_OK))
    {
      luax_dump_stack (L);
      ERROR ("%s", lua_tostring (L, -1));
    }
}
