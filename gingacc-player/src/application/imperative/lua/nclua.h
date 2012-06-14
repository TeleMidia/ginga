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

typedef int nclua_bool_t;

typedef enum _nclua_status_s
{
  NCLUA_STATUS_SUCCESS = 0,     /* no error has occurred */
  NCLUA_STATUS_NO_MEMORY,       /* out of memory */
  NCLUA_STATUS_NULL_POINTER,    /* NULL pointer */
  NCLUA_STATUS_BAD_ARGUMENT,    /* bad argument */
  NCLUA_STATUS_NOT_IMPLEMENTED, /* not implemented */
  NCLUA_STATUS_FILE_NOT_FOUND,  /* file not found */
  NCLUA_STATUS_INVALID_STATUS,  /* invalid value for nclua_status_t */
  NCLUA_STATUS_LAST_STATUS      /* total number of status values */
} nclua_status_t;

NCLUA_PUBLIC const char *
nclua_status_to_string (nclua_status_t status);

typedef struct _nclua_t nclua_t;
typedef void (*nclua_destroy_func_t) (void *);

NCLUA_PUBLIC nclua_t *
nclua_create (void);

NCLUA_PUBLIC void
nclua_destroy (nclua_t *nc);

NCLUA_PUBLIC nclua_t *
nclua_reference (nclua_t *nc);

NCLUA_PUBLIC int
nclua_get_reference_count (nclua_t *nc);

NCLUA_PUBLIC void
nclua_lock (nclua_t *nc);

NCLUA_PUBLIC void
nclua_unlock (nclua_t *nc);

NCLUA_PUBLIC void
nclua_set_user_data (nclua_t *nc, void *user_data,
                     nclua_destroy_func_t destroy);

NCLUA_PUBLIC void *
nclua_get_user_data (nclua_t *nc);

/* DEPRECATED */

NCLUA_PUBLIC void *
nclua_get_lua_state (nclua_t *nc);

NCLUA_PUBLIC nclua_t *
nclua_get_nclua_state (void *L);

NCLUA_END_DECLS

#endif /* NCLUA_H */
