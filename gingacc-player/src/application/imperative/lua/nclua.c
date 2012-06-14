/* nclua.c -- NCLua state handle.
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
#include <stdlib.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include <pthread.h>

#include "nclua.h"
#include "nclua-util-private.h"

/* NCLua state handle.  */
struct _nclua_t
{
  /* Current status.  */
  nclua_status_t status;

  /* Reference counter.  */
  int ref_count;

  /* Attached user-data.  */
  void *user_data;

  /* Used to release the attached user_data.  */
  nclua_destroy_func_t destroy_user_data;

  /* Used to sync access to state.  */
  pthread_mutex_t *mutex;

  /* Lua state associated with this NCLua state.  */
  lua_State *L;
};

/* Key of the NCLua state in Lua registry.  */
static const int _NCLUA_REGISTRY_KEY;
#define NCLUA_REGISTRY_KEY ((void *)(&_NCLUA_REGISTRY_KEY))

/* Stores the NCLua state NC into to the Lua state L.  */

static inline void
_nclua_set_state (lua_State *L, nclua_t *nc)
{
  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata (L, NCLUA_REGISTRY_KEY);
  lua_pushlightuserdata (L, (void *) nc);
  lua_rawset (L, -3);
  lua_pop (L, 1);
}

/* Returns the NCLua state associated with Lua state L.  */

static inline nclua_t *
_nclua_get_state (lua_State *L)
{
  nclua_t *nc;

  lua_pushvalue (L, LUA_REGISTRYINDEX);
  lua_pushlightuserdata (L, NCLUA_REGISTRY_KEY);
  lua_rawget (L, -2);

  nc = lua_touserdata (L, -1);
  assert (nc != NULL);

  lua_pop (L, 2);
  return nc;
}

#define DEFINE_NULL_STATE(status) { status, -1, NULL, NULL, NULL, NULL }
static const nclua_t _nclua_null[] =
{
  DEFINE_NULL_STATE (NCLUA_STATUS_NO_MEMORY),
};

static nclua_t *
_nclua_create_in_error (nclua_status_t status)
{
  nclua_t *nc;

  assert (status != NCLUA_STATUS_SUCCESS);

  nc = (nclua_t *) &_nclua_null[status - NCLUA_STATUS_NO_MEMORY];
  assert (status == nc->status);

  return nc;
}

#define _nclua_destroy_user_data(nc)                                    \
  NCLUA_STMT_BEGIN                                                      \
  {                                                                     \
    if ((nc)->user_data != NULL && (nc)->destroy_user_data != NULL)     \
      (nc)->destroy_user_data ((nc)->user_data);                        \
  }                                                                     \
  NCLUA_STMT_END

#define _nclua_destroy(nc)                      \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    if ((nc) != NULL)                           \
      {                                         \
        if ((nc)->L != NULL)                    \
          {                                     \
            lua_close ((nc)->L);                \
          }                                     \
        if ((nc)->mutex != NULL)                \
          {                                     \
            mutex_fini ((nc)->mutex);           \
            free ((nc)->mutex);                 \
          }                                     \
        _nclua_destroy_user_data (nc);          \
        free (nc);                              \
      }                                         \
  }                                             \
  NCLUA_STMT_END

#define _nclua_is_valid(nc)   (((nc) != NULL) && (nc)->ref_count > 0)
#define _nclua_is_invalid(nc) (!_nclua_is_valid (nc))

/* Custom panic function.  Revokes all pending locks.  */

static int
_nclua_atpanic (lua_State *L)
{
  nclua_t *nc = _nclua_get_state (L);
  mutex_unlockall (nc->mutex);
  return 0;
}

/* Mutex.  */

#define mutex_init(m)                                           \
  NCLUA_STMT_BEGIN                                              \
  {                                                             \
    pthread_mutexattr_t attr;                                   \
    pthread_mutexattr_init (&attr);                             \
    pthread_mutexattr_settype (&attr, PTHREAD_MUTEX_RECURSIVE); \
    pthread_mutex_init (m, &attr);                              \
    pthread_mutexattr_destroy (&attr);                          \
  }                                                             \
  NCLUA_STMT_END

#define mutex_fini(m)                           \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    pthread_mutex_destroy (m);                  \
  }                                             \
  NCLUA_STMT_END

#define mutex_lock(m)                           \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    assert (pthread_mutex_lock (m) == 0);       \
  }                                             \
  NCLUA_STMT_END

#define mutex_unlock(m)                         \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    assert (pthread_mutex_unlock (m) == 0);     \
  }                                             \
  NCLUA_STMT_END

#define mutex_unlockall(m)                      \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    while (pthread_mutex_unlock (m) == 0);      \
  }                                             \
  NCLUA_STMT_END

/* Creates an empty NCLua state and returns a pointer to it.  The caller
   owns the state and should call nclua_destroy() when done with it.

   This function always returns a valid pointer, but it will return an
   invalid state if an error, such as out-of-memory, occurs.  You can use
   nclua_status() to check for this.  */

nclua_t *
nclua_create (void)
{
  nclua_t *nc = NULL;
  lua_State *L = NULL;
  pthread_mutex_t *mutex = NULL;

  nc = (nclua_t *) malloc (sizeof (*nc));
  if (unlikely (nc == NULL))
    goto error_no_memory;

  mutex = (pthread_mutex_t *) malloc (sizeof (*mutex));
  if (unlikely (mutex == NULL))
    goto error_no_memory;

  mutex_init (mutex);
  nc->mutex = mutex;

  L = luaL_newstate ();
  if (unlikely (L == NULL))
    goto error_no_memory;

  lua_atpanic (L, _nclua_atpanic);
  luaL_openlibs (L);
  _nclua_set_state (L, nc);

  nc->status = NCLUA_STATUS_SUCCESS;
  nc->ref_count = 1;
  nc->user_data = NULL;
  nc->destroy_user_data = NULL;
  nc->mutex = mutex;
  nc->L = L;

  return nc;

 error_no_memory:
  _nclua_destroy (nc);
  return _nclua_create_in_error (NCLUA_STATUS_NO_MEMORY);
}

/* Decreases the reference count of state NC by one.  If the result is zero,
   then NC and all associated resources are freed.  */

void
nclua_destroy (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return;

  nclua_lock (nc);
  if (--nc->ref_count > 0)
    {
      nclua_unlock (nc);
      return;
    }
  _nclua_destroy (nc);
}

/* Increases the reference count on state NC by one.  This prevents NC from
   being destroyed until a matching call to nclua_destroy() is made.  The
   number of references to a nclua_t can be get using
   nclua_get_reference_count().

   This function returns the referenced nclua_t.  */

nclua_t *
nclua_reference (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return;

  nclua_lock (nc);
  nc->ref_count++;
  nclua_unlock (nc);

  return nc;
}

/* Returns the reference count of state NC.  */

int
nclua_get_reference_count (nclua_t *nc)
{
  int n;

  if (unlikely (_nclua_is_invalid (nc)))
    return 0;

  nclua_lock (nc);
  n = nc->ref_count;
  nclu_unlock (nc);

  return n;
}

/* Locks state.  */

void inline
nclua_lock (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return;
  mutex_lock (nc->mutex);
}

/* Unlocks state.  */

void inline
nclua_unlock (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return;
  mutex_unlock (nc->mutex);
}

/* Attach user data to state NC.

   If USER_DATA is non-NULL, attach it to NC; the previous user data value
   is be released, if any.  Otherwise, if USER_DATA is NULL, destroy the
   attached user data.

   If DESTROY is non-NULL, then DESTROY is used to release USER_DATA when NC
   is released.  */

NCLUA_PUBLIC void
nclua_set_user_data (nclua_t *nc, void *user_data,
                     nclua_destroy_func_t destroy)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return;

  nclua_lock (nc);

  _nclua_destroy_user_data (nc);
  if (user_data == NULL)
    return;

  nc->user_data = user_data;
  nc->destroy_user_data = destroy;

  nclua_unlock (nc);
}

/* Returns user data previously attached to state NC.
   If no user data has been attached, returns NULL.  */

NCLUA_PUBLIC void *
nclua_get_user_data (nclua_t *nc)
{
  void *user_data;

  if (unlikely (_nclua_is_invalid (nc)))
    return;

  nclua_lock (nc);
  user_data = nc->user_data;
  nclua_unlock (nc);

  return nc->user_data;
}

/* ********************************************************************** *
 * *                             DEPRECATED                             * *
 * *                Remove all this as soon as possible!                * *
 * ********************************************************************** */

NCLUA_DEPRECATED void *
nclua_get_lua_state (nclua_t *nc)
{
  lua_State *L;

  nclua_lock (nc);
  L = nc->L;
  nclua_unlock (nc);

  return (void *) L;
}

NCLUA_DEPRECATED nclua_t *
nclua_get_nclua_state (void *L)
{
  return _nclua_get_state ((lua_State *) L);
}
