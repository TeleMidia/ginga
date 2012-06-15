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

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "nclua.h"
#include "nclua-private.h"
#include "nclua-luax-private.h"

/* Define the Lua registry key for the NCLua state.  */
_NCLUA_MAGIC (nclua);

/* NCLua state handle.  */
struct _nclua_t
{
  nclua_status_t status;        /* current status */
  int ref_count;                /* reference counter */
  void *user_data;              /* attached user data */
  nclua_destroy_func_t destroy; /* release user data */
  lua_State *lua_state;              /* Lua state */
};

#define DEFINE_NIL_STATE(status)                      \
  {                                                   \
    status,                     /* status */          \
    -1,                         /* ref_count */       \
    NULL,                       /* user_data */       \
    NULL,                       /* destroy_fn */      \
    NULL,                       /* lua_state */       \
  }

static const nclua_t nclua_nil[] =
{
  DEFINE_NIL_STATE (NCLUA_STATUS_NO_MEMORY),
  DEFINE_NIL_STATE (NCLUA_STATUS_NULL_POINTER),
  DEFINE_NIL_STATE (NCLUA_STATUS_FILE_NOT_FOUND),
  DEFINE_NIL_STATE (NCLUA_STATUS_INVALID_STATUS),
};

static inline nclua_t *
_nclua_create_in_error (nclua_status_t status)
{
  nclua_t *nc;

  assert (status != NCLUA_STATUS_SUCCESS);
  nc = (nclua_t *) &nclua_nil[status - NCLUA_STATUS_NO_MEMORY];
  assert (status == nc->status);

  return nc;
}

#define _nclua_destroy_user_data(nc)                            \
  NCLUA_STMT_BEGIN                                              \
  {                                                             \
    if ((nc)->user_data != NULL && (nc)->destroy != NULL)       \
      (nc)->destroy ((nc)->user_data);                          \
  }                                                             \
  NCLUA_STMT_END

#define _nclua_destroy(nc)                              \
  NCLUA_STMT_BEGIN                                      \
  {                                                     \
    if ((nc) != NULL)                                   \
      {                                                 \
        if ((nc)->lua_state != NULL)                    \
          {                                             \
            _nclua_event_close ((nc)->lua_state);       \
            lua_close ((nc)->lua_state);                \
          }                                             \
        _nclua_destroy_user_data (nc);                  \
        free (nc);                                      \
      }                                                 \
  }                                                     \
  NCLUA_STMT_END

#define _nclua_is_valid(nc)   (((nc) != NULL) && (nc)->ref_count > 0)
#define _nclua_is_invalid(nc) (!_nclua_is_valid (nc))


/* Exported private functions.  */

/* Returns the Lua state associated with NCLua state NC.  */

lua_State *
_nclua_get_lua_state (nclua_t *nc)
{
  assert (nc->lua_state != NULL);
  return nc->lua_state;
}

/* Returns the NCLua state associated with Lua state L.  */

nclua_t *
_nclua_get_nclua_state (lua_State *L)
{
  nclua_t *nc;

  _NCLUA_GET_MODULE_DATA (L, nclua);
  nc = (nclua_t *) lua_touserdata (L, -1);
  assert (nc != NULL);
  lua_pop (L, 1);

  return nc;
}

static void
ncluax_print_error (lua_State *L, int level, const char *prefix,
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
  ncluax_print_error (L, level, "NCLUA ERROR\t", format, args);
  va_end (args);
}

/* Outputs Lua warning at level LEVEL to standard error.  */

void
_nclua_warning (lua_State *L, int level, const char *format, ...)
{
  va_list args;
  va_start (args, format);
  ncluax_print_error (L, level, "NCLUA Warning\t", format, args);
  va_end (args);
}


/* The NCLua API.  */

/* Provides a human-readable description of a nclua_status_t.
   Returns a string representation of STATUS.  */

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

    case NCLUA_STATUS_FILE_NOT_FOUND:
      return "file not found";

    case NCLUA_STATUS_INVALID_STATUS:
      return "invalid value for nclua_status_t";

    case NCLUA_STATUS_LAST_STATUS:
    default:
      return "<unknown error status>";
    }
  ASSERT_NOT_REACHED;
}

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
  nclua_status_t status;

  nc = (nclua_t *) malloc (sizeof (*nc));
  if (unlikely (nc == NULL))
    goto error_no_memory;

  memset (nc, 0, sizeof (*nc));

  L = luaL_newstate ();
  if (unlikely (L == NULL))
    goto error_no_memory;

  luaL_openlibs (L);

  lua_pushlightuserdata (L, (void *) nc);
  _NCLUA_SET_MODULE_DATA (L, nclua);

  status = _nclua_event_open (L);
  if (unlikely (status == NCLUA_STATUS_NO_MEMORY))
    goto error_no_memory;

  nc->status = NCLUA_STATUS_SUCCESS;
  nc->ref_count = 1;
  nc->user_data = NULL;
  nc->destroy = NULL;
  nc->lua_state = L;

  return nc;

 error_no_memory:
  _nclua_destroy (nc);
  return _nclua_create_in_error (NCLUA_STATUS_NO_MEMORY);
}

/* Decreases the reference count of NCLua state NC by one.
   If the result is zero, frees NC and all associated resources.  */

void
nclua_destroy (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return;

  if (--nc->ref_count > 0)
    return;

  _nclua_destroy (nc);
}

/* Increases the reference count on NCLua state NC by one.  This prevents NC
   from being destroyed until a matching call to nclua_destroy() is made.
   The number of references to a nclua_t can be get using
   nclua_get_reference_count().

   This function returns the referenced nclua_t.  */

nclua_t *
nclua_reference (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return nc;

  nc->ref_count++;

  return nc;
}

/* Returns the reference count of NCLua state NC.  */

int
nclua_get_reference_count (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return 0;

  return nc->ref_count;
}

/* Attaches user data to NCLua state NC.

   If USER_DATA is non-NULL, attach it to NC; the previous user data value
   is be released, if any.  Otherwise, if USER_DATA is NULL, destroys the
   attached user data.

   If DESTROY is non-NULL, then DESTROY is used to release USER_DATA when NC
   is released.  */

void
nclua_set_user_data (nclua_t *nc, void *user_data,
                     nclua_destroy_func_t destroy)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return;

  _nclua_destroy_user_data (nc);
  if (user_data == NULL)
    return;

  nc->user_data = user_data;
  nc->destroy = destroy;
}

/* Returns user data previously attached to NCLua state NC.
   If no user data has been attached, returns NULL.  */

void *
nclua_get_user_data (nclua_t *nc)
{
  if (unlikely (_nclua_is_invalid (nc)))
    return NULL;

  return nc->user_data;
}


/* DEPRECATED: This will be removed as soon as possible.  */

void *
nclua_get_lua_state (nclua_t *nc)
{
  return (void *) _nclua_get_lua_state (nc);
}

nclua_t *
nclua_get_nclua_state (void *L)
{
  return _nclua_get_nclua_state ((lua_State *) L);
}
