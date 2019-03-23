/* Copyright (C) 2014-2018 Free Software Foundation, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#ifndef AUX_LUA_H
#define AUX_LUA_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <assert.h>
#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#if defined __GNUC__ && __GNUC__ > 2
# define LUAX_GNUC_UNUSED __attribute__((__unused__))
#else
# define LUAX_GNUC_UNUSED
#endif

#define LUAX_STMT_START do
#define LUAX_STMT_END   while (0)

static void LUAX_GNUC_UNUSED
luax_getfield (lua_State *L, int i, const char *k)
{
  int top;
  const char *begin;
  const char *end;

  top = lua_gettop (L);
  lua_pushvalue (L, i);

  for (begin = end = k; *end != '\0'; end++)
    {
      if (*end == '.')
        {
          assert (end - begin > 0);
          lua_pushlstring (L, begin, (size_t) (end - begin));
          lua_gettable (L, -2);
          begin = ++end;
        }
    }
  assert (end - begin > 0);
  lua_pushlstring (L, begin, (size_t) (end - begin));
  lua_gettable (L, -2);

  lua_insert (L, top + 1);
  lua_settop (L, top + 1);
}

#define luax_newmetatable(L, tname)                     \
  LUAX_STMT_START                                       \
  {                                                     \
    assert (luaL_newmetatable ((L), (tname)) != 0);     \
    lua_pushvalue ((L), -1);                            \
    lua_setfield ((L), -2, "__index");                  \
    lua_pushliteral ((L), "not your business");         \
    lua_setfield ((L), -2, "__metatable");              \
  }                                                     \
  LUAX_STMT_END

#define luax_optudata(L, i, tname)                      \
  LUAX_STMT_START                                       \
  {                                                     \
    switch (lua_type ((L), (i)))                        \
      {                                                 \
      case LUA_TUSERDATA:                               \
        if (!luaL_testudata ((L), (i), (tname)))        \
          {                                             \
            luaL_getmetatable ((L), (tname));           \
            lua_insert ((L), (i));                      \
          }                                             \
        break;                                          \
      case LUA_TTABLE:                                  \
        luaL_getmetatable ((L), (tname));               \
        if (!lua_rawequal ((L), (i), -1))               \
          {                                             \
            lua_insert ((L), (i));                      \
            break;                                      \
          }                                             \
        lua_pop ((L), 1);                               \
        break;                                          \
      default:                                          \
        luaL_getmetatable ((L), (tname));               \
        lua_insert ((L), (i));                          \
      }                                                 \
  }                                                     \
  LUAX_STMT_END

#define luax_pushupvalue(L, i)\
  lua_pushvalue ((L), lua_upvalueindex (i))

#define luax_setbooleanfield(L, i, name, value)\
  _luax_setxfield (lua_pushboolean, (L), (i), (name), (value))

#define luax_setintegerfield(L, i, name, value)\
  _luax_setxfield (lua_pushinteger, (L), (i), (name), (value))

#define luax_setnumberfield(L, i, name, value)\
  _luax_setxfield (lua_pushnumber, (L), (i), (name), (value))

#define luax_setstringfield(L, i, name, value)\
  _luax_setxfield (lua_pushstring, (L), (i), (name), (value))

#define _luax_setxfield(func, L, i, name, value)        \
  LUAX_STMT_START                                       \
  {                                                     \
    lua_pushvalue ((L), (i));                           \
    func ((L), (value));                                \
    lua_setfield ((L), -2, (name));                     \
    lua_pop ((L), 1);                                   \
  }                                                     \
  LUAX_STMT_END

/* Pops the table on top of stack and sets it as the registry of M, i.e.,
   stores the table as the value associated with address M in the Lua
   registry.  */
#define luax_mregistry_create(L, m)                     \
  LUAX_STMT_START                                       \
  {                                                     \
    assert (lua_type ((L), -1) == LUA_TTABLE);          \
    lua_pushvalue ((L), LUA_REGISTRYINDEX);             \
    lua_insert ((L), -2);                               \
    lua_rawsetp ((L), -2, (m));                         \
    lua_pop ((L), 1);                                   \
  }                                                     \
  LUAX_STMT_END

/* Destroy M's registry, i.e., removes the table associated with M from Lua
   registry.  */
#define luax_mregistry_destroy(L, m)            \
  LUAX_STMT_START                               \
  {                                             \
    lua_pushvalue ((L), LUA_REGISTRYINDEX);     \
    lua_pushnil ((L));                          \
    lua_rawsetp ((L), -2, (m));                 \
    lua_pop ((L), 1);                           \
  }                                             \
  LUAX_STMT_END

/* Pushes M's registry onto stack.  */
#define luax_mregistry_get(L, m)                \
  LUAX_STMT_START                               \
  {                                             \
    lua_pushvalue ((L), LUA_REGISTRYINDEX);     \
    lua_rawgetp ((L), -1, (m));                 \
    lua_remove ((L), -2);                       \
  }                                             \
  LUAX_STMT_END

/* Pushes onto stack the value of the given field in M's registry.  */
#define luax_mregistry_getfield(L, m, field)    \
  LUAX_STMT_START                               \
  {                                             \
    luax_mregistry_get ((L), (m));              \
    luax_getfield ((L), -1, (field));           \
    lua_remove ((L), -2);                       \
  }                                             \
  LUAX_STMT_END

/* Pops the value on top of stack and sets it as the value associated with
   the given field in M's registry.  */
#define luax_mregistry_setfield(L, m, field)    \
  LUAX_STMT_START                               \
  {                                             \
    luax_mregistry_get ((L), (m));              \
    lua_insert ((L), -2);                       \
    lua_setfield ((L), -2, (field));            \
    lua_pop ((L), 1);                           \
  }                                             \
  LUAX_STMT_END

/* Debugging.  */
#define luax_dump_value(L, i)\
  (_luax_dump_value ((L), (i)), putc ('\n', stderr))

#define luax_dump_table(L, i)\
  (_luax_dump_table ((L), (i), 1), putc ('\n', stderr))

#define luax_dump_stack(L)\
  _luax_dump_stack ((L), 1)

static void LUAX_GNUC_UNUSED
_luax_dump_value (lua_State *L, int index)
{
  index = lua_absindex (L, index);
  switch (lua_type (L, index))
    {
    case LUA_TBOOLEAN:
      fputs (strbool (lua_toboolean (L, index)), stderr);
      break;

    case LUA_TNIL:
      fputs ("<nil>", stderr);
      break;

    case LUA_TNONE:
      fputs ("<none>", stderr);
      break;

    case LUA_TNUMBER:
      fprintf (stderr, "%g", lua_tonumber (L, index));
      break;

    case LUA_TSTRING:
      fprintf (stderr, "'%s'", lua_tostring (L, index));
      break;

    case LUA_TFUNCTION:
    case LUA_TLIGHTUSERDATA:
    case LUA_TTABLE:
    case LUA_TTHREAD:
    case LUA_TUSERDATA:
      fprintf (stderr, "<%s:%p>", lua_typename (L, lua_type (L, index)),
               lua_topointer (L, index));
      break;

    default:
      g_assert_not_reached ();
    }
}

static void LUAX_GNUC_UNUSED
_luax_dump_table (lua_State *L, int index, int depth)
{
  int first = 1;
  int table = lua_absindex (L, index);

  fflush (stdout);
  fprintf (stderr, "<%s:%p:{", lua_typename (L, LUA_TTABLE),
           lua_topointer (L, table));
  lua_pushnil (L);
  while (lua_next (L, table) != 0)
    {
      if (first)
        {
          first = 0;
        }
      else
        {
          fputc (',', stderr);
        }
      _luax_dump_value (L, -2);
      fputc ('=', stderr);

      if (lua_istable (L, -1) && depth > 0)
        {
          _luax_dump_table (L, -1, depth - 1);
        }
      else
        {
          _luax_dump_value (L, -1);
        }
      lua_pop (L, 1);
    }
  fputs ("}>", stderr);
  fflush (stderr);
}

static void LUAX_GNUC_UNUSED
_luax_dump_stack (lua_State *L, int depth)
{
  int i;

  fflush (stdout);
  fprintf (stderr, "Lua stack dump (%p):\n", (void *) L);

  for (i = lua_gettop (L); i >= 1; i--)
    {
      fprintf (stderr, "#%d\t", i);
      if (lua_type (L, i) == LUA_TTABLE && depth > 0)
        {
          _luax_dump_table (L, i, depth - 1);
        }
      else
        {
          _luax_dump_value (L, i);
        }
      fputc ('\n', stderr);
    }
  fflush (stderr);
}

#endif /* AUX_LUA_H */
