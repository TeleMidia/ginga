/* nclua-internal.c -- Internal declarations.
   Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia

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

#include "nclua-internal.h"     /* specification */

const char *
nclua_status_to_string (nclua_status_t status)
{
  switch (status)
    {
    case NCLUA_STATUS_SUCCESS:
      return "no error has occurred";

    case NCLUA_STATUS_NO_MEMORY:
      return "out of memory";

    case NCLUA_STATUS_NULL_POINTER:
      return "NULL pointer";

    case NCLUA_STATUS_BAD_ARGUMENT:
      return "bad argument";

    case NCLUA_STATUS_NOT_IMPLEMENTED:
      return "not implemented";

    case NCLUA_STATUS_FILE_NOT_FOUND:
      return "file not found";

    case NCLUA_STATUS_INVALID_STATUS:
      return "invalid value for nclua_status_t";

    default:
    case NCLUA_STATUS_LAST_STATUS:
      return "<unknown error status>";
    }
}

void
__nclua_error_at_line (const char *prefix, const char *file, int line,
                       const char *func, const char *fmt, ...)
{
  nclua_bool_t sp = FALSE;
  va_list args;

  fflush (stdout);

  if (prefix != NULL)
    {
      fputs (prefix, stderr);
      sp = TRUE;
    }

  if (file != NULL)
    {
      fprintf (stderr, "%s:", file);
      if (line > 0)
        fprintf (stderr, "%d:", line);
      sp = TRUE;
    }

  if (func != NULL)
    {
      fprintf (stderr, "%s:", func);
      sp = TRUE;
    }

  if (sp)
    fputc (' ', stderr);

  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);

  fputc ('\n', stderr);
  fflush (stderr);
}

void
_nclua_lerror_at_line (lua_State *L, int level, const char *prefix,
                       const char *fmt, ...)
{
  nclua_bool_t sp = FALSE;
  va_list args;
  const char *where;

  luaL_where (L, level);
  where = lua_tostring (L, -1);
  lua_pop (L, 1);

  fflush (stdout);

  if (prefix != NULL)
    {
      fputs (prefix, stderr);
      sp = TRUE;
    }

  if (where != NULL)
    {
      fputs (where, stderr);
      sp = TRUE;
    }

  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);

  fputc ('\n', stderr);
  fflush (stderr);
}

/* Returns the name of the current Lua function.  */

const char *
luax_currlfunc (lua_State *L)
{
  lua_Debug ar;

  lua_getstack (L, 0, &ar);
  lua_getinfo (L, "n", &ar);
  return ar.name;
}

/* Returns the absolute value of INDEX in Lua stack.  */

int
luax_absindex (lua_State *L, int index)
{
  int n = lua_gettop (L);
  return (index < 0) ? max (index + n + 1, 0) : min (index, n);
}

/* Sets t[K] to nil, where t is the table at INDEX.  */

void
luax_unsetfield (lua_State *L, int index, const char *k)
{
  int t = luax_absindex (L, index);
  lua_pushnil (L);
  lua_setfield (L, t, k);
}

/* If t[K] is an integer (where t is the table at INDEX),
   sets *RESULT to t[K] and returns true.  Otherwise, returns false.  */

nclua_bool_t
luax_getintfield (lua_State *L, int index, const char *k, int *result)
{
  nclua_bool_t status = TRUE;
  int t = luax_absindex (L, index);

  lua_getfield (L, t, k);
  if (unlikely (!lua_isnumber (L, -1)))
    {
      status = FALSE;
      goto tail;
    }
  *result = lua_tointeger (L, -1);

 tail:
  lua_pop (L, 1);
  return status;
}

/* If t[K] is a number (where t is the table at INDEX),
   sets *RESULT to t[K] and returns true.  Otherwise, returns false.  */

nclua_bool_t
luax_getnumberfield (lua_State *L, int index, const char *k, double *result)
{
  nclua_bool_t status = TRUE;
  int t = luax_absindex (L, index);

  lua_getfield (L, t, k);
  if (unlikely (!lua_isnumber (L, -1)))
    {
      status = FALSE;
      goto tail;
    }
  *result = (double) lua_tonumber (L, -1);

 tail:
  lua_pop (L, 1);
  return status;
}

/* If t[K] is a string (where t is the table at INDEX),
   sets *RESULT to t[K] and returns true.  Otherwise, returns false.  */

nclua_bool_t
luax_getstringfield (lua_State *L, int index, const char *k,
                  const char **result)
{
  nclua_bool_t status = TRUE;
  int t = luax_absindex (L, index);

  lua_getfield (L, t, k);
  if (unlikely (!lua_isstring (L, -1)))
    {
      status = FALSE;
      goto tail;
    }
  *result = lua_tostring (L, -1);
  assert (*result != NULL);

 tail:
  lua_pop (L, 1);
  return status;
}

/* Inserts element at top of stack in position POS of table at INDEX.
   Other elements are shifted up to open space, if necessary.  */

extern void
luax_tableinsert (lua_State *L, int index, int pos)
{
  int t = luax_absindex (L, index);
  int n = lua_objlen (L, t);
  pos = min (max (pos, 1), n + 1);

  /* Shift up other elements, before inserting.  */
  if (pos <= n)
    {
      int i;
      for (i = n; i >= pos; i--)
        {
          lua_pushinteger (L, i);
          lua_gettable (L, t);

          lua_pushinteger (L, i + 1);
          lua_insert (L, -2);
          lua_settable (L, t);
        }
    }

  /* Insert element into table.  */
  lua_pushinteger (L, pos);
  lua_pushvalue (L, -2);
  lua_settable (L, t);
}

/* Removes element in position POS of table at INDEX.
   Other elements are shifted down to close the space, if necessary.  */

extern void
luax_tableremove (lua_State *L, int index, int pos)
{
  int i;
  int t = luax_absindex (L, index);
  int n = lua_objlen (L, t);

  if (unlikely (pos < 1 || pos > n))
    return;                     /* nothing to do */

  for (i = pos; i <= n; i++)
    {
      lua_pushinteger (L, i + 1);
      lua_gettable (L, t);

      lua_pushinteger (L, i);
      lua_insert (L, -2);
      lua_settable (L, t);
    }
}

/* Do a shallow dump of value at index INDEX to file FILE.  */

static void
dump_value (lua_State *L, int index, FILE *file)
{
  int i = luax_absindex (L, index);
  switch (lua_type (L, i))
    {
    case LUA_TSTRING:
      fprintf (file, "'%s'", lua_tostring (L, i));
      break;
    case LUA_TBOOLEAN:
      fputs (lua_toboolean (L, i) ? "true" : "false", file);
      break;
    case LUA_TNUMBER:
      fprintf (file, "%g", lua_tonumber (L, i));
      break;
    case LUA_TNIL:
    case LUA_TNONE:
      fputs ("<nil>", file);
      break;
    default:
      fprintf (file, "<%s:%p>", lua_typename (L, lua_type (L, i)),
               lua_topointer (L, i));
      break;
    }
}

/* Dumps the contents of table at INDEX to file FILE.

   The DEPTH parameter controls the maximum number of recursive calls
   permitted when dumping nested tables.  If DEPTH < 0, then the maximum
   depth is indefinite -- beware that this may cause infinite loops.  */

void
luax_dump_table_to_file (lua_State *L, int index, int depth, FILE *file)
{
  nclua_bool_t first = TRUE;
  int t = luax_absindex (L, index);

  fflush (NULL);
  fprintf (file, "<table:%p:{", lua_topointer (L, t));

  lua_pushnil (L);
  while (lua_next (L, t) != 0)
    {
      if (!first)
        fputc (',', file);
      else
        first = FALSE;

      dump_value (L, -2, file);
      fputc ('=', file);

      if (lua_type (L, -1) == LUA_TTABLE && depth != 0)
        luax_dump_table_to_file (L, -1, depth - 1, file);
      else
        dump_value (L, -1, file);

      lua_pop (L, 1);
    }

  fputs ("}>", file);
  fflush (file);
}

/* Dumps the contents of Lua state to file FILE.

   The DEPTH parameter controls the maximum number of recursive calls
   permitted when dumping nested tables.  If DEPTH < 0, then the maximum
   depth is indefinite -- beware that this may cause infinite loops.  */

void
luax_dump_stack_to_file (lua_State *L, int depth, FILE *file)
{
  int i;
  int n;

  fflush (NULL);
  fputs ("NCLua stack dump:\n", file);

  n = lua_gettop (L);
  for (i = n; i >= 1; i--)
    {
      fprintf (file, "#%d\t", i);

      if (lua_type (L, i) == LUA_TTABLE)
        luax_dump_table_to_file (L, i, depth, file);
      else
        dump_value (L, i, file);

      fputc ('\n', file);
    }

  fflush (file);
}
