/* nclua-luax.c -- Auxiliary Lua functions.
   Copyright (C) 2012 PUC-Rio/Laboratorio TeleMidia

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your option)
any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc., 51
Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <assert.h>
#include <stdio.h>

#include <lua.h>
#include <lauxlib.h>

#include "nclua.h"
#include "nclua-private.h"
#include "nclua-luax-private.h"

/* Sets t[KEY] to nil, where t is the table at index INDEX.  */

void
ncluax_unsetfield (lua_State *L, int index, const char *key)
{
  lua_pushnil (L);
  lua_setfield (L, ncluax_abs (L, index), key);
}

#define _NCLUAX_GETXFIELD_BODY(lua_isx, lua_tox)        \
  {                                                     \
    nclua_bool_t status = FALSE;                        \
    lua_getfield (L, ncluax_abs (L, index), key);       \
    if (likely (lua_isx (L, -1)))                       \
      {                                                 \
        status = TRUE;                                  \
        *value = lua_tox (L, -1);                       \
      }                                                 \
    lua_pop (L, 1);                                     \
    return status;                                      \
  }

/* If t[KEY] is an integer, stores it in *VALUE and returns true.
   Otherwise, returns false.  */

nclua_bool_t
ncluax_getintfield (lua_State *L, int index, const char *key, int *value)
{
  _NCLUAX_GETXFIELD_BODY (lua_isnumber, lua_tointeger)
}

/* If t[KEY] is a number, stores it in *VALUE and returns true.
   Otherwise, returns false.  */

nclua_bool_t
ncluax_getnumberfield (lua_State *L, int index,
                       const char *key, double *value)
{
  _NCLUAX_GETXFIELD_BODY (lua_isnumber, lua_tonumber)
}

/* If t[KEY] is a string, stores it in *VALUE and returns true.
   Otherwise, returns false.  */

nclua_bool_t
ncluax_getstringfield (lua_State *L, int index, const char *key,
                       const char **value)
{
  _NCLUAX_GETXFIELD_BODY (lua_isstring, lua_tostring)
}

static void
ncluax_tableinsert_tail (lua_State *L, int index, int position,
                         void (*lua_gettable_func) (lua_State *, int),
                         void (*lua_settable_func) (lua_State *, int))
{
  int table = ncluax_abs (L, index);
  int size = lua_objlen (L, table);
  position = range (position, 1, size + 1);

  /* Shift up other elements, before inserting.  */

  if (position <= size)
    {
      int i;
      for (i = size; i >= position; i--)
        {
          lua_pushinteger (L, i);
          lua_gettable_func (L, table);

          lua_pushinteger (L, i + 1);
          lua_insert (L, -2);
          lua_settable_func (L, table);
        }
    }

  /* Insert the new element into table.  */

  lua_pushinteger (L, position);
  lua_insert (L, -2);
  lua_settable_func (L, table);
}

/* Inserts the value at top of stack in position POSITION of table at
   index INDEX.  Other elements are shifted up to open space, if
   necessary.  This function pops the value from stack.  */

void
ncluax_tableinsert (lua_State *L, int index, int position)
{
  ncluax_tableinsert_tail (L, index, position, lua_gettable, lua_settable);
}

/* Similar to ncluax_tableinsert(), but does a raw access;
   i.e., without metamethods.  */

void
ncluax_rawinsert (lua_State *L, int index, int position)
{
  ncluax_tableinsert_tail (L, index, position, lua_rawget, lua_rawset);
}

static void
ncluax_tableremove_tail (lua_State *L, int index, int position,
                         void (*lua_gettable_func) (lua_State *, int),
                         void (*lua_settable_func) (lua_State *, int))
{
  int i;
  int table = ncluax_abs (L, index);
  int size = lua_objlen (L, table);

  if (unlikely (position < 1 || position > size))
    {
      lua_pushnil (L);
      return;                   /* nothing to do */
    }

  lua_pushinteger (L, position);
  lua_gettable_func (L, table);

  for (i = position; i <= size; i++)
    {
      lua_pushinteger (L, i + 1);
      lua_gettable_func (L, table);

      lua_pushinteger (L, i);
      lua_insert (L, -2);
      lua_settable_func (L, table);
    }
}

/* Removes the element in position POSITION of table at index INDEX.
   Other elements are shifted down to close the space, if necessary.
   This function pushes onto stack the removed value.  */

void
ncluax_tableremove (lua_State *L, int index, int position)
{
  ncluax_tableremove_tail (L, index, position, lua_gettable, lua_settable);
}

/* Similar to ncluax_tableremove(), but does a raw access;
   i.e., without metamethods.  */

void
ncluax_rawremove (lua_State *L, int index, int position)
{
  ncluax_tableremove_tail (L, index, position, lua_rawget, lua_rawset);
}

/* Returns the name of the current Lua function.  */

const char *
ncluax_get_function_name (lua_State *L, int level)
{
  lua_Debug ar;
  assert (lua_getstack (L, level, &ar));
  assert (lua_getinfo (L, "n", &ar));
  return ar.name;
}

/* Do a shallow dump of value at index INDEX to standard error.  */

static void
ncluax_dump_value (lua_State *L, int index)
{
  index = ncluax_abs (L, index);
  switch (lua_type (L, index))
    {
    case LUA_TBOOLEAN:
      fputs (lua_toboolean (L, index) ? "true" : "false", stderr);
      break;
    case LUA_TNUMBER:
      fprintf (stderr, "%g", lua_tonumber (L, index));
      break;
    case LUA_TNIL:
    case LUA_TNONE:
      fputs ("<nil>", stderr);
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
      ASSERT_NOT_REACHED;
    }
}

/* Dumps the contents of table at INDEX to standard error.

   The DEPTH parameter controls the maximum number of recursive calls
   permitted when dumping nested tables.  If DEPTH < 0, then this number is
   assumed to be unlimited; beware that this may case infinite loops.  */

void
ncluax_dump_table (lua_State *L, int index, int depth)
{
  nclua_bool_t first = TRUE;
  int table = ncluax_abs (L, index);

  fflush (stdout);
  fprintf (stderr, "<%s:%p:{", lua_typename (L, LUA_TTABLE),
           lua_topointer (L, table));

  lua_pushnil (L);
  while (lua_next (L, table) != 0)
    {
      if (likely (!first))
        fputc (',', stderr);
      else
        first = FALSE;

      ncluax_dump_value (L, -2);
      fputc ('=', stderr);

      if (lua_type (L, -1) == LUA_TTABLE && depth != 0)
        ncluax_dump_table (L, -1, depth - 1);
      else
        ncluax_dump_value (L, -1);

      lua_pop (L, 1);
    }

  fputs ("}>", stderr);
  fflush (stderr);
}

/* Dumps the contents of the Lua stack to standard error.

   The DEPTH parameter controls the maximum number of recursive calls
   permitted when dumping nested tables.  If DEPTH < 0, then this number is
   assumed to be unlimited; beware that this may case infinite loops.  */

void
ncluax_dump_stack (lua_State *L, int depth)
{
  int i;
  int n;

  fflush (stdout);
  fprintf (stderr, "NCLua stack dump (%p):\n", (void *) L);

  n = lua_gettop (L);
  for (i = n; i >= 1; i--)
    {
      fprintf (stderr, "#%d\t", i);

      if (lua_type (L, i) == LUA_TTABLE && depth != 0)
        ncluax_dump_table (L, i, depth);
      else
        ncluax_dump_value (L, i);

      fputc ('\n', stderr);
    }

  fflush (stderr);
}
