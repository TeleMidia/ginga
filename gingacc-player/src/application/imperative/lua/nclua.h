/* nclua.h -- The NCLua engine.
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

#ifndef NCLUA_H
#define NCLUA_H

#if defined LIBNCLUA_BUILDING && defined HAVE_VISIBILITY
# define NCLUA_PUBLIC extern __attribute__((__visibility__("default")))
#elif defined LIBNCLUA_BUILDING && defined _MSC_VER
# define NCLUA_PUBLIC extern __declspec(dllexport)
#elif defined _MSC_VER
# define NCLUA_PUBLIC extern __declspec(dllimport)
#else
# define NCLUA_PUBLIC extern
#endif

#ifdef __cplusplus
# define NCLUA_BEGIN_DECLS  extern "C" {
# define NCLUA_END_DECLS    }
#else
# define NCLUA_BEGIN_DECLS
# define NCLUA_END_DECLS
#endif

NCLUA_BEGIN_DECLS

#include <lua.h>

typedef int nclua_bool_t;

typedef enum
{
  NCLUA_STATUS_SUCCESS = 0,       /* no error has occurred */
  NCLUA_STATUS_NO_MEMORY,         /* out of memory */
  NCLUA_STATUS_NULL_POINTER,      /* NULL pointer */
  NCLUA_STATUS_INVALID_LUA_STATE, /* invalid lua_State */
  NCLUA_STATUS_INVALID_STATUS,    /* invalid value for nclua_status_t */
  NCLUA_STATUS_LAST_STATUS        /* total number of status values */
} nclua_status_t;

NCLUA_PUBLIC const char *
nclua_status_to_string (nclua_status_t status);

typedef struct _nclua_t nclua_t;

typedef struct
{
  int unused;
} nclua_user_data_key_t;

typedef void (*nclua_destroy_func_t) (void *);

NCLUA_PUBLIC nclua_t *
nclua_create (void);

NCLUA_PUBLIC nclua_t *
nclua_create_for_lua_state (lua_State *L);

NCLUA_PUBLIC void
nclua_destroy (nclua_t *nc);

NCLUA_PUBLIC nclua_status_t
nclua_status (nclua_t *nc);

NCLUA_PUBLIC nclua_t *
nclua_reference (nclua_t *nc);

NCLUA_PUBLIC int
nclua_get_reference_count (nclua_t *nc);

NCLUA_PUBLIC void
nclua_set_user_data (nclua_t *nc, nclua_user_data_key_t *key,
                     void *user_data, nclua_destroy_func_t destroy);

NCLUA_PUBLIC void *
nclua_get_user_data (nclua_t *nc, nclua_user_data_key_t *key);

NCLUA_PUBLIC lua_State *
nclua_get_lua_state (nclua_t *nc);

NCLUA_PUBLIC nclua_t *
nclua_get_nclua_state (lua_State *L);

NCLUA_PUBLIC void
nclua_send (nclua_t *nc, lua_State *L);

NCLUA_PUBLIC void
nclua_receive (nclua_t *nc, lua_State *L);

NCLUA_PUBLIC void
nclua_cycle (nclua_t *nc);

/* DEPRECATED */

NCLUA_PUBLIC void
nclua_sendx (nclua_t *nc);

NCLUA_END_DECLS

#endif /* NCLUA_H */
