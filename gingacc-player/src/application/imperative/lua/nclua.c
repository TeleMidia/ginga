/* nclua.c -- NCLua state.
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

#include <assert.h>
#include <limits.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "nclua.h"
#include "nclua-private.h"
#include "nclua-luax-private.h"

/* Define the registry key for the NCLua Store (cf. nclua-private.h).  */
const int _nclua_magic;

/* The NCLua State structure.  */
struct _nclua_t
{
  nclua_status_t status;        /* current status */
  int ref_count;                /* reference counter */
  lua_State *lua_state;         /* associated Lua state */
  nclua_bool_t close_lua_state; /* true if lua_state must be closed */
};

#define DEFINE_NIL_STATE(status)                      \
  {                                                   \
    status,                     /* status */          \
    -1,                         /* ref_count */       \
    NULL,                       /* lua_state */       \
    FALSE,                      /* close_lua_state */ \
  }

static const nclua_t __nclua_nil[] =
{
  DEFINE_NIL_STATE (NCLUA_STATUS_NO_MEMORY),
  DEFINE_NIL_STATE (NCLUA_STATUS_NULL_POINTER),
  DEFINE_NIL_STATE (NCLUA_STATUS_INVALID_LUA_STATE),
  DEFINE_NIL_STATE (NCLUA_STATUS_INVALID_STATUS),
};

static inline nclua_t *
__nclua_create_in_error (nclua_status_t status)
{
  nclua_t *nc;

  assert (status != NCLUA_STATUS_SUCCESS);
  nc = (nclua_t *) &__nclua_nil[status - NCLUA_STATUS_NO_MEMORY];
  assert (status == nc->status);

  return nc;
}

#define __nclua_is_valid(nc)   (((nc) != NULL) && (nc)->ref_count > 0)
#define __nclua_is_invalid(nc) (!__nclua_is_valid (nc))


/* Exported private functions.  */

static void
__nclua_print_error (lua_State *L, int level, const char *prefix,
                     const char *format, va_list args)
{
  nclua_bool_t space = FALSE;
  const char *where;

  fflush (stdout);

  if (prefix != NULL)
    {
      fputs (prefix, stderr);
      space = TRUE;
    }

  luaL_where (L, level);
  where = lua_tostring (L, -1);
  lua_pop (L, 1);
  if (where != NULL)
    {
      fputs (where, stderr);
      space = TRUE;
    }

  vfprintf (stderr, format, args);
  fputc ('\n', stderr);
  fflush (stderr);
}

/* Outputs Lua error message at level LEVEL to standard error.  */

void
_nclua_error (lua_State *L, int level, const char *format, ...)
{
  va_list args;
  va_start (args, format);
  __nclua_print_error (L, level, "NCLUA ERROR\t", format, args);
  va_end (args);
}

/* Outputs Lua warning at level LEVEL to standard error.  */

void
_nclua_warning (lua_State *L, int level, const char *format, ...)
{
  va_list args;
  va_start (args, format);
  __nclua_print_error (L, level, "NCLUA Warning\t", format, args);
  va_end (args);
}


/* The NCLua API.  */

/* Creates an empty NCLua state for the provided Lua state.  The caller owns
   the NCLua state and should call nclua_destroy() when done with it.

   Note that if you create the NCLua state using this function, then
   nclua_destroy() shall NOT close the associated Lua state.

   This function always returns a valid pointer, but it will return an
   invalid state if an error, such as out-of-memory, occurs.  You can use
   nclua_status() to check for this.  */

nclua_t *
nclua_create_for_lua_state (lua_State *L)
{
  nclua_t *nc = NULL;
  nclua_status_t status;

  if (unlikely (L == NULL))
    return __nclua_create_in_error (NCLUA_STATUS_NULL_POINTER);

  /* Check if the Lua state is valid.  */
  _nclua_get_store (L);
  if (unlikely (!lua_isnil (L, -1)))
    {
      lua_pop (L, 1);
      return __nclua_create_in_error (NCLUA_STATUS_INVALID_LUA_STATE);
    }
  lua_pop (L, 1);

  /* Allocate the NCLua state.  */
  nc = (nclua_t *) malloc (sizeof (*nc));
  if (unlikely (nc == NULL))
    return __nclua_create_in_error (NCLUA_STATUS_NO_MEMORY);

  memset (nc, 0, sizeof (*nc));

  /* Create and initialize the NCLua store table.  */
  _nclua_create_and_set_store (L);

  lua_createtable (L, 0, 0);
  _nclua_set_store_data (L, _NCLUA_STORE_EMPTY_TABLE_KEY);

  lua_pushlightuserdata (L, (void *) nc);
  _nclua_set_store_data (L, _NCLUA_STORE_STATE_KEY);

  lua_newtable (L);
  _nclua_set_store_data (L, _NCLUA_STORE_USER_DATA_KEY);

  status = _nclua_event_open (L);
  if (unlikely (status != NCLUA_STATUS_SUCCESS))
    {
      lua_pushnil (L);
      _nclua_set_store (L);
      free (nc);
      return __nclua_create_in_error (status);
    }

  nc->status = NCLUA_STATUS_SUCCESS;
  nc->ref_count = 1;
  nc->lua_state = L;
  nc->close_lua_state = FALSE;

  return nc;
}

/* Creates an empty NCLua state and returns a pointer to it.  The caller
   owns the state and should call nclua_destroy() when done with it.

   Note that this call creates a new Lua state and associates it with the
   returned NCLua state.  If you create the NCLua state using this function,
   then nclua_destroy() shall automatically close the associated Lua state.

   This function always returns a valid pointer, but it will return an
   invalid state if an error, such as out-of-memory, occurs.  You can use
   nclua_status() to check for this.  */

nclua_t *
nclua_create (void)
{
  nclua_t *nc;
  lua_State *L;

  L = luaL_newstate ();
  if (unlikely (L == NULL))
    return __nclua_create_in_error (NCLUA_STATUS_NO_MEMORY);

  luaL_openlibs (L);

  nc = nclua_create_for_lua_state (L);
  if (unlikely (nclua_status (nc) != NCLUA_STATUS_SUCCESS))
    {
      lua_close (L);
      return nc;
    }

  nc->close_lua_state = TRUE;

  return nc;
}

/* Decreases the reference count of NCLua state NC by one.
   If the result is zero, frees NC and all associated resources.  */

void
nclua_destroy (nclua_t *nc)
{
  lua_State *L;

  if (unlikely (__nclua_is_invalid (nc)))
    return;

  if (--nc->ref_count > 0)
    return;

  L = nclua_get_lua_state (nc);

  /* Release NCLua Event data.  */
  _nclua_event_close (L);

  _nclua_get_store_data (L, _NCLUA_STORE_USER_DATA_KEY);
  assert (lua_istable (L, -1));

  /* Release user data */
  lua_pushnil (L);
  while (lua_next (L, -1) != 0)
    {
      void *user_data;
      nclua_destroy_func_t destroy;

      lua_rawgeti (L, -1, 1);
      user_data = lua_touserdata (L, -1);

      lua_rawgeti (L, -2, 1);
      destroy = lua_touserdata (L, -1);

      if (user_data != NULL && destroy != NULL)
        destroy (user_data);

      lua_pop (L, 3);
    }

  /* Release store table.  */
  lua_pushnil (L);
  _nclua_set_store (L);

  if (nc->close_lua_state)
    lua_close (L);

  free (nc);
}

/* Returns the current status of NCLua state NC.  */

nclua_status_t
nclua_status (nclua_t *nc)
{
  return nc->status;
}

/* Increases the reference count on NCLua state NC by one.  This prevents NC
   from being destroyed until a matching call to nclua_destroy() is made.
   The number of references to a nclua_t can be get using
   nclua_get_reference_count().

   This function returns the referenced nclua_t.  */

nclua_t *
nclua_reference (nclua_t *nc)
{
  if (unlikely (__nclua_is_invalid (nc)))
    return nc;

  nc->ref_count++;

  return nc;
}

/* Returns the reference count of NCLua state NC.  */

int
nclua_get_reference_count (nclua_t *nc)
{
  if (unlikely (__nclua_is_invalid (nc)))
    return 0;

  return nc->ref_count;
}

/* Attaches user data to NCLua state NC.

   KEY is the address of a nclua_user_data_t to attach the user data to.

   If USER_DATA is non-NULL, attach it to NC using the specified key.  The
   previous user data value is be released, if any.  Otherwise, if USER_DATA
   is NULL, destroys the attached user data.

   If DESTROY is non-NULL, then DESTROY is used to release USER_DATA when NC
   is released.  */

void
nclua_set_user_data (nclua_t *nc, nclua_user_data_key_t *key,
                     void *user_data, nclua_destroy_func_t destroy)
{
  lua_State *L;
  int saved_top;

  if (unlikely (__nclua_is_invalid (nc)))
    return;

  L = nclua_get_lua_state (nc);
  saved_top = lua_gettop (L);

  _nclua_get_store_data (L, _NCLUA_STORE_USER_DATA_KEY);
  lua_pushlightuserdata (L, (void *) key);
  lua_rawget (L, -2);

  /* Delete current user data.  */
  if (user_data == NULL)
    {
      if (lua_isnil (L, -1))
        goto tail;              /* nothing to do */

      lua_rawgeti (L, -1, 1);
      user_data = lua_touserdata (L, -1);
      lua_pop (L, 1);

      if (user_data == NULL)
        goto tail;              /* nothing to do */

      lua_rawgeti (L, -1, 2);
      destroy = lua_touserdata (L, -1);
      lua_pop (L, 1);

      if (destroy == NULL)
        goto tail;              /* nothing to do */

      destroy (user_data);
    }

  /* Replace current user data by USER_DATA
     and the associated destroy function by DESTROY.  */
  else if (lua_istable (L, -1))
    {
      void *prev_user_data;

      lua_rawgeti (L, -1, 1);
      prev_user_data = lua_touserdata (L, -1);
      lua_pop (L, 1);

      if (prev_user_data != NULL)
        {
          nclua_destroy_func_t prev_destroy;

          lua_rawgeti (L, -1, 2);
          prev_destroy = lua_touserdata (L, -1);
          lua_pop (L, 1);

          if (prev_destroy)
            prev_destroy (prev_user_data);
        }

      lua_pushlightuserdata (L, (void *) user_data);
      lua_rawseti (L, -2, 1);

      lua_pushlightuserdata (L, (void *) destroy);
      lua_rawseti (L, -2, 2);
    }

  /* No previous user data.  Attach the new user data USER_DATA
     and the associated destroy function DESTROY.  */
  else if (lua_isnil (L, -1))
    {
      lua_pop (L, 1);
      lua_pushlightuserdata (L, (void *) key);
      lua_createtable (L, 2, 0);
      lua_rawset (L, -3);

      lua_pushlightuserdata (L, (void *) key);
      lua_rawget (L, -2);

      lua_pushlightuserdata (L, user_data);
      lua_rawseti (L, -2, 1);

      lua_pushlightuserdata (L, destroy);
      lua_rawseti (L, -2, 2);
    }

  /* Oops...  */
  else
    {
      ASSERT_NOT_REACHED;
    }

 tail:
  lua_settop (L, saved_top);
}

/* Returns user data previously attached to NCLua state NC using the
   specified key.  If no user data has been attached, returns NULL.  */

void *
nclua_get_user_data (nclua_t *nc, nclua_user_data_key_t *key)
{
  lua_State *L;
  int saved_top;
  void *user_data;

  if (unlikely (__nclua_is_invalid (nc)))
    return NULL;

  L = nclua_get_lua_state (nc);
  saved_top = lua_gettop (L);

  _nclua_get_store_data (L, _NCLUA_STORE_USER_DATA_KEY);
  lua_pushlightuserdata (L, (void *) key);
  lua_rawget (L, -2);
  if (unlikely (lua_isnil (L, -1)))
    {
      user_data = NULL;
      goto tail;
    }

  lua_rawgeti (L, -1, 1);
  user_data = lua_touserdata (L, -1);

 tail:
  lua_settop (L, saved_top);
  return user_data;
}

/* Returns the Lua state associated with NCLua state NC.  */

lua_State *
nclua_get_lua_state (nclua_t *nc)
{
  assert (nc->lua_state != NULL);
  return nc->lua_state;
}

/* Returns the NCLua state associated with Lua state L.  */

nclua_t *
nclua_get_nclua_state (lua_State *L)
{
  int saved_top;
  nclua_t *nc;

  saved_top = lua_gettop (L);

  _nclua_get_store_data (L, _NCLUA_STORE_STATE_KEY);
  assert (lua_islightuserdata (L, -1));

  nc = (nclua_t *) lua_touserdata (L, -1);
  assert (nc != NULL);

  lua_settop (L, saved_top);

  return nc;
}
