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
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _MSC_VER
# include <windows.h>
# include <mmsystem.h>
# define __nclua_epoch_t          DWORD
# define __nclua_epoch_null       0
# define __nclua_epoch_setnull(e) (e = 0)
# define __nclua_epoch_isnull(e)  (e == 0)
#else
# include <sys/time.h>
# define __nclua_epoch_t          struct timeval
# define __nclua_epoch_null       {0, 0}
# define __nclua_epoch_setnull(e) ((e).tv_sec = 0, (e).tv_usec = 0)
# define __nclua_epoch_isnull(e)  ((e).tv_sec == 0 && (e).tv_usec == 0)
#endif

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "nclua.h"
#include "nclua-private.h"
#include "nclua-luax-private.h"

/* Registry key for the NCLua registry.  */
const int _nclua_magic;

/* NCLua State data.  */
struct _nclua_t
{
  nclua_status_t status;        /* current status */
  int ref_count;                /* reference counter */
  __nclua_epoch_t epoch;        /* tick value of the first cycle */
  lua_State *lua_state;         /* associated Lua state */
  nclua_bool_t close_lua_state; /* true if lua_state must be closed */
};

#define DEFINE_NIL_STATE(status)                        \
  {                                                     \
    status,                     /* status */            \
    -1,                         /* ref_count */         \
    __nclua_epoch_null,         /* epoch */             \
    NULL,                       /* lua_state */         \
    FALSE,                      /* close_lua_state */   \
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

/* Resets the up-time value for NCLua state.  */

void
_nclua_reset_uptime (nclua_t *nc)
{
#ifdef _MSC_VER
  timeBeginPeriod (1);
  nc->epoch = timeGetTime ();
#else
  gettimeofday (&nc->epoch, NULL);
#endif
}

/* Returns the time delay (in ms) since the first nclua_cycle call.  */

unsigned int
_nclua_get_uptime (nclua_t *nc)
{
#ifdef _MSC_VER
  DWORD now;
  DWORD start;
  DWORD uptime;
  start = nc->epoch;
  now = timeGetTime ();
  if (now < start)
    {
      uptime = ((~(DWORD) 0) - start) + now;
    }
  else
    {
      uptime = now - start;
    }
  return (unsigned long) uptime;
#else
  unsigned int uptime;
  struct timeval now;
  struct timeval start = nc->epoch;
  assert (gettimeofday (&now, NULL) == 0);
  uptime = (unsigned int) ((now.tv_sec - start.tv_sec) * 1000
                           + (now.tv_usec - start.tv_usec) / 1000);
  return uptime;
#endif
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
  _nclua_get_registry (L);
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

  /* Create and initialize the NCLua registry table.  */
  _nclua_create_registry (L);

  lua_createtable (L, 0, 0);
  _nclua_set_registry_data (L, _NCLUA_REGISTRY_EMPTY_TABLE);

  lua_pushlightuserdata (L, (void *) nc);
  _nclua_set_registry_data (L, _NCLUA_REGISTRY_STATE);

  lua_newtable (L);
  _nclua_set_registry_data (L, _NCLUA_REGISTRY_USER_DATA_TABLE);

  status = _nclua_event_open (L);
  if (unlikely (status != NCLUA_STATUS_SUCCESS))
    {
      _nclua_destroy_registry (L);
      free (nc);
      return __nclua_create_in_error (status);
    }

  nc->status = NCLUA_STATUS_SUCCESS;
  nc->ref_count = 1;
  __nclua_epoch_setnull (nc->epoch);
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

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_USER_DATA_TABLE);
  assert (lua_istable (L, -1));

  /* Release user data */
  lua_pushnil (L);
  while (lua_next (L, -2) != 0)
    {
      void *user_data;
      nclua_destroy_func_t destroy;

      lua_rawgeti (L, -1, 1);
      user_data = lua_touserdata (L, -1);

      lua_rawgeti (L, -2, 2);
      destroy = lua_touserdata (L, -1);

      if (user_data != NULL && destroy != NULL)
        destroy (user_data);

      lua_pop (L, 3);
    }
  lua_pop (L, 1);

  /* Release registry table.  */
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_EMPTY_TABLE);
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_STATE);
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_USER_DATA_TABLE);
  _nclua_destroy_registry (L);

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

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_USER_DATA_TABLE);
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

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_USER_DATA_TABLE);
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

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_STATE);
  assert (lua_islightuserdata (L, -1));

  nc = (nclua_t *) lua_touserdata (L, -1);
  assert (nc != NULL);

  lua_settop (L, saved_top);

  return nc;
}

/* Cycle the NCLua engine once.  */

void
nclua_cycle (nclua_t *nc)
{
  lua_State *L;
  int i;
  int n;

  if (unlikely (__nclua_epoch_isnull (nc->epoch)))
    {
      _nclua_reset_uptime (nc); /* first cycle */
    }

  L = nclua_get_lua_state (nc);

  _nclua_get_registry_data (L, _NCLUA_REGISTRY_INPUT_QUEUE);
  n = lua_objlen (L, -1);
  if (n == 0)
    goto tail;                  /* nothing to do */

  /* Cleanup input queue to avoid handling
     the events generated in the current cycle.  */
  _nclua_unset_registry_data (L, _NCLUA_REGISTRY_INPUT_QUEUE);

  for (i = 1; i < n; i++)
    {
      lua_rawgeti (L, -1, i);
      _nclua_notify (L);
      lua_pop (L, 1);
    }

 tail:
  lua_pop (L, 1);
}
