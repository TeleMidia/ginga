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

bool
LuaAPI::GValue_to (lua_State *L, int i, GValue *value)
{
  g_return_val_if_fail (L != NULL, false);
  g_return_val_if_fail (value != NULL, false);

  switch (lua_type (L, i))
    {
    case LUA_TBOOLEAN:
      {
        g_value_init (value, G_TYPE_BOOLEAN);
        g_value_set_boolean (value, lua_toboolean (L, i));
        break;
      }
    case LUA_TNUMBER:
      {
        if (lua_isinteger (L, i))
          {
            g_value_init (value, G_TYPE_INT64);
            g_value_set_int64 (value, luaL_checkinteger (L, i));
          }
        else
          {
            g_value_init (value, G_TYPE_DOUBLE);
            g_value_set_double (value, luaL_checknumber (L, i));
          }
        break;
      }
    case LUA_TSTRING:
      {
        g_value_init (value, G_TYPE_STRING);
        g_value_set_string (value, luaL_checkstring (L, i));
        break;
      }
    default:
      {
        return false;
      }
    }

  return true;
}

bool
LuaAPI::GValue_push (lua_State *L, const GValue *value)
{
  g_return_val_if_fail (L != NULL, false);
  g_return_val_if_fail (value != NULL, false);

  if (G_VALUE_HOLDS (value, G_TYPE_BOOLEAN))
    {
      lua_pushboolean (L, g_value_get_boolean (value));
    }
  else if (G_VALUE_HOLDS (value, G_TYPE_INT64))
    {
      lua_pushinteger (L, g_value_get_int64 (value));
    }
  else if (G_VALUE_HOLDS (value, G_TYPE_DOUBLE))
    {
      lua_pushnumber (L, g_value_get_double (value));
    }
  else if (G_VALUE_HOLDS (value, G_TYPE_STRING))
    {
      lua_pushstring (L, g_value_get_string (value));
    }
  else
    {
      return false;
    }

  return true;
}

// Private.

LUAAPI_CHUNK_DEFN (initMt);
LUAAPI_CHUNK_DEFN (Document_initMt);
LUAAPI_CHUNK_DEFN (Object_initMt);
LUAAPI_CHUNK_DEFN (Context_initMt);
LUAAPI_CHUNK_DEFN (Media_initMt);
LUAAPI_CHUNK_DEFN (Composition_initMt);
LUAAPI_CHUNK_DEFN (StateMachine_initMt);
LUAAPI_CHUNK_DEFN (Player_initMt);
LUAAPI_CHUNK_DEFN (traceMt);

const char *LuaAPI::_DOCUMENT      = "Ginga.Document";
const char *LuaAPI::_CONTEXT       = "Ginga.Context";
const char *LuaAPI::_SWITCH        = "Ginga.Switch";
const char *LuaAPI::_MEDIA         = "Ginga.Media";
const char *LuaAPI::_STATE_MACHINE = "Ginga.StateMachine";
const char *LuaAPI::_PLAYER        = "Ginga.Player";

const struct luaL_Reg LuaAPI::_funcs[]
  = {{"_debug",   LuaAPI::_l_debug},
     {"_warning", LuaAPI::_l_warn},
     {"_error",   LuaAPI::_l_error},
     {NULL, NULL}};

int
LuaAPI::_l_debug (lua_State *L)
{
  g_debug ("%s", luaL_checkstring (L, -1));
  return 0;
}

int
LuaAPI::_l_warn (lua_State *L)
{
  g_warning ("%s", luaL_checkstring (L, -1));
  return 0;
}

int G_GNUC_NORETURN
LuaAPI::_l_error (lua_State *L)
{
  g_error ("%s", luaL_checkstring (L, -1));
}

static void
xloadbuffer (lua_State *L, const char *chunk, size_t len, const char *name)
{
  int err;
  err = luaL_loadbuffer (L, chunk, len, name);
  if (unlikely (err != LUA_OK))
    {
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
      ERROR ("%s", lua_tostring (L, -1));
    }
}

bool
LuaAPI::_loadLuaWrapperMt (lua_State *L, const char *name,
                           const luaL_Reg *const funcs[],
                           const Chunk *const chunks[])
{
  size_t i;

  luaL_getmetatable (L, name);
  if (!lua_isnil (L, -1))
    {
      lua_pop (L, 1);
      return false;
    }

  lua_pop (L, 1);
  luaL_newmetatable (L, name);

  if (funcs != NULL)
    {
      for (i = 0; funcs[i] != NULL; i++)
        luaL_setfuncs (L, funcs[i], 0);
    }

  if (chunks != NULL)
    {
      for (i = 0; chunks[i] != NULL; i++)
        {
          xloadbuffer (L, chunks[i]->text, chunks[i]->len, chunks[i]->name);
          lua_pushvalue (L, -2);
          xpcall (L, 1, 0);
        }
    }

  lua_pop (L, 1);
  return true;
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
  if (unlikely (luaL_getmetafield (L, -1, name) == LUA_TNIL))
    {
      ERROR ("no method '%s' for object %p", name, ptr);
    }

  lua_insert (L, (-nargs) -2);
  lua_insert (L, (-nargs) -1);

  if (unlikely (lua_pcall (L, nargs + 1, nresults, 0) != LUA_OK))
    {
      ERROR ("method '%s' of object %p: %s", name, ptr,
             lua_tostring (L, -1));
    }
}
