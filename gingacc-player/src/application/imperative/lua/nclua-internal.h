/* nclua-internal.h -- Internal declarations.
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

#ifndef _NCLUA_INTERNAL_H
#define _NCLUA_INTERNAL_H 1

#ifdef __cplusplus
# define NCLUA_BEGIN_DECLS  extern "C" {
# define NCLUA_END_DECLS    }
#else
# define NCLUA_BEGIN_DECLS
# define NCLUA_END_DECLS
#endif

NCLUA_BEGIN_DECLS

/* #define NDEBUG   1           -- disable assertions */
/* #define NWARNINGS 1          -- disable warnings */
/* #define NEXTENSIONS 1        -- disable extensions */

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/* Compiler Features  */

#if defined __GNUC__ && !defined __STRICT_ANSI__ && !defined __cplusplus
# define NCLUA_STMT_BEGIN  (void)(
# define NCLUA_STMT_END    )
#elif defined sun || defined __sun__
# define NCLUA_STMT_BEGIN  if (1)
# define NCLUA_STMT_END    else (void)0
#else
# define NCLUA_STMT_BEGIN  do
# define NCLUA_STMT_END    while (0)
#endif

#if defined __GNUC__ && defined __GNUC_MINOR__
# define NCLUA_GNUC_PREREQ(major, minor) \
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((major) << 16) + (minor))
#else
# define NCLUA_GNUC_PREREQ(major, minor) 0
#endif

#if NCLUA_GNUC_PREREQ (2,5)
# define NCLUA_NORETURN         __attribute__((__noreturn__))
# define NCLUA_PRINTF_FORMAT(fmt, va) \
  __attribute__((__format__(__printf__, fmt, va)))
# define NCLUA_UNUSED           __attribute__((__unused__))
#else
# define NCLUA_NORETURN
# define NCLUA_PRINTF_FORMAT
# define NCLUA_UNUSED
#endif

#if NCLUA_GNUC_PREREQ (2,96)
# define NCLUA_LIKELY(expr)     __builtin_expect ((expr), 1)
# define NCLUA_UNLIKELY(expr)   __builtin_expect ((expr), 0)
#else
# define NCLUA_LIKELY(expr)     (expr)
# define NCLUA_UNLIKELY(expr)   (expr)
#endif

#if NCLUA_GNUC_PREREQ (3,0)
# define NCLUA_ALWAYS_INLINE    inline __attribute__((__always_inline__))
# define NCLUA_CONST            __attribute__((__const__))
# define NCLUA_MALLOC           __attribute__((__malloc__))
# define NCLUA_PURE             __attribute__((__pure__))
#else
# define NCLUA_ALWAYS_INLINE    inline
# define NCLUA_CONST
# define NCLUA_MALLOC
# define NCLUA_PURE
#endif

#if NCLUA_GNUC_PREREQ (3,3)
# define NCLUA_DEPRECATED       __attribute__((__deprecated__))
# define NCLUA_NONNULL(args)    __attribute__((__nonnull__ args))
#else
# define NCLUA_DEPRECATED
# define NCLUA_NONNULL(args)
#endif

#if NCLUA_GNUC_PREREQ (3,4)
# define NCLUA_ARG_UNUSED(arg)  arg NCLUA_UNUSED
# define NCLUA_USE_RESULT       __attribute__((__warn_unused_result__))
#else
# define NCLUA_ARG_UNUSED(arg)
# define NCLUA_USE_RESULT
#endif

/* Pre-Processor Token Manipulation */

#define _NCLUA_STRINGIFY(arg)   #arg
#define NCLUA_STRINGIFY(arg)    _NCLUA_STRINGIFY (arg)

#define _NCLUA_CONCAT(a, b)     a##b
#define NCLUA_CONCAT(a, b)      _NCLUA_CONCAT (a, b)

/* Compile-time Assertions */

#define _NCLUA_COMPILE_TIME_ASSERT1(cond, line) \
  typedef int tm_compile_time_assertion_at_line_##line##_failed[(cond)?1:-1]
#define _NCLUA_COMPILE_TIME_ASSERT0(cond, line) \
  _NCLUA_COMPILE_TIME_ASSERT1 (cond, line) /* macro-expand LINE */
#define NCLUA_COMPILE_TIME_ASSERT(cond) \
  _NCLUA_COMPILE_TIME_ASSERT0 (cond, __LINE__)

/* Miscellanea  */

typedef int nclua_bool_t;

#ifndef FALSE
# define FALSE 0
#endif
#ifndef TRUE
# define TRUE  1
#endif

#define nelementsof(x)      (sizeof (x) / sizeof (x[0]))
#define integralof(x)       (((char *)(x)) - ((char *) 0))
#define pointerof(x)        ((void *)((char *) 0 + (x)))
#ifndef offsetof
# define offsetof(st, fld)  ((size_t)(&((st *) 0)->fld))
#endif

#define isodd(n)            ((n) & 1)
#define iseven(n)           (!isodd (n))
#define sign(x)             ((x) >= 0.0 ? 1 : -1)
#define max(a, b)           (((a) > (b)) ? (a) : (b))
#define min(a, b)           (((a) < (b)) ? (a) : (b))
#define streq(a, b)         ((*(a) == *(b)) && strcmp (a, b) == 0)

#define DECONST(t, x)       ((t)(size_t)(const void *)(x))
#define DEVOLATILE(t, x)    ((t)(size_t)(volatile void *)(x))
#define DEQUALIFY(t, x)     ((t)(size_t)(const volatile void *)(x))

#define likely(x)           NCLUA_LIKELY (x)
#define unlikely(x)         NCLUA_UNLIKELY (x)

/* Error Handling */

/* nclua_status_t is used to indicate errors.  */
typedef enum _nclua_status_s
{
  NCLUA_STATUS_SUCCESS = 0,     /* no error has occurred */
  NCLUA_STATUS_NO_MEMORY,       /* out of memory */
  NCLUA_STATUS_NULL_POINTER,    /* NULL pointer */
  NCLUA_STATUS_BAD_ARGUMENT,    /* bad argument */
  NCLUA_STATUS_NOT_IMPLEMENTED, /* not implemented */
  NCLUA_STATUS_FILE_NOT_FOUND,  /* file not found */
  NCLUA_STATUS_INVALID_STATUS,  /* invalid value for nclua_status_t */

  /* The following is a special value indicating the
     number of status values defined in this enumeration.  */

  NCLUA_STATUS_LAST_STATUS
} nclua_status_t;

/* Provides a human-readable description of a nclua_status_t.
   Returns a string representation of STATUS.  */
extern const char *
nclua_status_to_string (nclua_status_t status);

/* Pushes onto stack a human-readable description of STATUS.  */
#define nclua_pushstatus(L, status) \
  lua_pushstring (L, nclua_status_to_string (status))

/* Message Logging */

#define NCLUA_ERROR_PREFIX  "NCLUA ERROR\t"
#define NCLUA_WARN_PREFIX   "NCLUA Warning\t"

extern void
__nclua_error_at_line (const char *prefix, const char *file, int line,
                      const char *func, const char *fmg, ...);
extern void
_nclua_lerror_at_line (lua_State *L, int level, const char *prefix,
                       const char *fmt, ...);

#define _nclua_error_at_line(prefix, fmt, ...)          \
  __nclua_error_at_line (prefix, __FILE__, __LINE__,    \
                         __func__, fmt, ## __VA_ARGS__)

/* Prints internal error message to standard error.  */
#define nclua_error(fmt, ...) \
  _nclua_error_at_line  (NCLUA_ERROR_PREFIX, fmt, ## __VA_ARGS__)

/* Prints error-in-script message to standard error.  */
#define nclua_lerror(L, level, fmt, ...) \
  _nclua_lerror_at_line (L, level, NCLUA_ERROR_PREFIX, fmt, ## __VA_ARGS__)

#ifndef NWARNINGS
/* Prints internal warning message to standard error.  */
# define nclua_warn(fmt, ...) \
  _nclua_error_at_line (NCLUA_WARN_PREFIX, fmt, ## __VA_ARGS__)

/* Prints warning-in-script message to standard error.  */
# define nclua_lwarn(L, level, fmt, ...) \
  _nclua_lerror_at_line (L, level, NCLUA_WARN_PREFIX, fmt, ## __VA_ARGS__)
#else
# define nclua_warn(fmt, ...)
# define nclua_lwarn(L, fmt, ...)
#endif /* !NWARNINGS */

/* Common warnings:  */
#define nclua_lwarn_extra_args(L) \
  nclua_lwarn (L, 1, "ignoring extra arguments to function")

/* Stack Manipulation */

extern const char *
luax_currlfunc (lua_State *L);

extern int
luax_absindex (lua_State *L, int index);

extern void
luax_unsetfield (lua_State *L, int index, const char *k);

extern nclua_bool_t
luax_getintfield (lua_State *L, int index, const char *k, int *result);

extern nclua_bool_t
luax_getnumberfield (lua_State *L, int index,
                     const char *k, double *result);

extern nclua_bool_t
luax_getstringfield (lua_State *L, int index,
                     const char *k, const char **result);

extern void
luax_tableinsert (lua_State *L, int index, int pos);

extern void
luax_tableremove (lua_State *L, int index, int pos);

extern void
luax_dump_stack_to_file (lua_State *L, int depth, FILE *file);

#define luax_dump_stack(L)     luax_dump_stack_to_file (L, -1, stderr)

extern void
luax_dump_table_to_file (lua_State *L, int index, int depth, FILE *file);

#define luax_dump_table(L, i)  luax_dump_table_to_file (L, i, -1, stderr)

NCLUA_END_DECLS

#endif /* _NCLUA_INTERNAL_H */
