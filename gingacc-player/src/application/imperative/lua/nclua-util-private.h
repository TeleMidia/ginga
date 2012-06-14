/* nclua-util-private.h -- Utility functions.
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

#ifndef NCLUA_UTIL_PRIVATE_H
#define NCLUA_UTIL_PRIVATE_H

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "nclua.h"
#include "nclua-compiler-private.h"

NCLUA_BEGIN_DECLS

#undef  FALSE
#define FALSE 0

#undef  TRUE
#define TRUE  1

#undef  likely
#define likely           NCLUA_LIKELY

#undef  unlikely
#define unlikely         NCLUA_UNLIKELY

#undef  arg_unused
#define arg_unused       NCLUA_ARG_UNUSED

#undef  arg_nonnull
#define arg_nonnull      NCLUA_NONNULL

#undef  nelementsof
#define nelementsof(x)   (sizeof (x) / sizeof (x[0]))

#undef  integralof
#define integralof(x)    (((char *)(x)) - ((char *) 0))

#undef  pointerof
#define pointerof(x)     ((void *)((char *) 0 + (x)))

#undef  isodd
#define isodd(n)         ((n) & 1)

#undef  iseven
#define iseven(n)        (!isodd (n))

#undef  sign
#define sign(x)          ((x) >= 0.0 ? 1 : -1)

#undef  max
#define max(a, b)        (((a) > (b)) ? (a) : (b))

#undef  min
#define min(a, b)        (((a) < (b)) ? (a) : (b))

#undef  streq
#define streq(a, b)      ((*(a) == *(b)) && strcmp (a, b) == 0)

#undef  strneq
#define streneq(a, b)    (!streq (a, b))

#undef  deconst
#define deconst(t, x)    ((t)(size_t)(const void *)(x))

#undef  devolatile
#define devolatile(t, x) ((t)(size_t)(volatile void *)(x))

#undef  dequalify
#define dequalify(t, x)  ((t)(size_t)(const volatile void *)(x))

#define _NCLUA_STRINGIFY(arg) #arg
#define NCLUA_STRINGIFY(arg)  _NCLUA_STRINGIFY (arg)

#define _NCLUA_CONCAT(a, b)   a##b
#define NCLUA_CONCAT(a, b)    _NCLUA_CONCAT (a, b)

#define ASSERT_NOT_REACHED                      \
  NCLUA_STMT_BEGIN                              \
  {                                             \
    assert (!"reached");                        \
  }                                             \
  NCLUA_STMT_END

#define _NCLUA_COMPILE_TIME_ASSERT1(cond, line) \
  typedef int tm_compile_time_assertion_at_line_##line##_failed[(cond)?1:-1]
#define _NCLUA_COMPILE_TIME_ASSERT0(cond, line) \
  _NCLUA_COMPILE_TIME_ASSERT1 (cond, line) /* macro-expand LINE */
#define NCLUA_COMPILE_TIME_ASSERT(cond) \
  _NCLUA_COMPILE_TIME_ASSERT0 (cond, __LINE__)

NCLUA_END_DECLS

#endif /* NCLUA_UTIL_PRIVATE_H */
