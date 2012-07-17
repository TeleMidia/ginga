/* nclua-luax-private.h -- Auxiliary Lua functions.
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

#ifndef NCLUA_LUAX_PRIVATE_H
#define NCLUA_LUAX_PRIVATE_H

#include "nclua.h"
#include "nclua-private.h"
#include "nclua-compiler-private.h"

NCLUA_BEGIN_DECLS

#include <lua.h>

/* Returns the absolute value of index INDEX in stack.  */
static NCLUA_ALWAYS_INLINE int
ncluax_abs (lua_State *L, int index)
{
  int n = lua_gettop (L);
  return (index < 0) ? max (index + n + 1, 0) : min (index, n);
}

/* Table.  */

NCLUA_PRIVATE void
ncluax_unsetfield (lua_State *L, int index, const char *key);

NCLUA_PRIVATE nclua_bool_t
ncluax_getintegerfield (lua_State *L, int index, const char *key,
                        int *value);

NCLUA_PRIVATE nclua_bool_t
ncluax_getnumberfield (lua_State *L, int index, const char *key,
                       double *value);

NCLUA_PRIVATE nclua_bool_t
ncluax_getstringfield (lua_State *L, int index, const char *key,
                       const char **value);

NCLUA_PRIVATE nclua_bool_t
ncluax_getuserdatafield (lua_State *L, int index, const char *key,
                         void **value);

NCLUA_PRIVATE void
ncluax_tableinsert (lua_State *L, int index, int position);

NCLUA_PRIVATE void
ncluax_tableremove (lua_State *L, int index, int position);

NCLUA_PRIVATE void
ncluax_rawinsert (lua_State *L, int index, int position);

NCLUA_PRIVATE void
ncluax_rawremove (lua_State *L, int index, int position);

NCLUA_PRIVATE void
ncluax_pushcopy (lua_State *L, int index);

/* Debug.  */

NCLUA_PRIVATE const char *
ncluax_get_function_name (lua_State *L, int level);

NCLUA_PRIVATE void
ncluax_dump_table (lua_State *L, int index, int depth);

NCLUA_PRIVATE void
ncluax_dump_stack (lua_State *L, int depth);

NCLUA_END_DECLS

#endif /* NCLUA_LUAX_PRIVATE_H */
