/* nclua-compiler-private.h -- Compiler features.
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

#ifndef NCLUA_COMPILER_PRIVATE_H
#define NCLUA_COMPILER_PRIVATE_H

#if HAVE_CONFIG_H
# include "config.h"
#endif

#include "nclua.h"

#if defined __GNUC__ && !defined __STRICT_ANSI__ && !defined __cplusplus
# define NCLUA_STMT_BEGIN (void)(
# define NCLUA_STMT_END   )
#elif defined sun || defined __sun__
# define NCLUA_STMT_BEGIN if (1)
# define NCLUA_STMT_END   else (void)0
#else
# define NCLUA_STMT_BEGIN do
# define NCLUA_STMT_END   while (0)
#endif

#if defined __GNUC__ && defined __GNUC_MINOR__
# define NCLUA_GNUC_PREREQ(major, minor) \
  ((__GNUC__ << 16) + __GNUC_MINOR__ >= ((major) << 16) + (minor))
#else
# define NCLUA_GNUC_PREREQ(major, minor) 0
#endif

#if NCLUA_GNUC_PREREQ (2,5)
# define NCLUA_NORETURN __attribute__((__noreturn__))
# define NCLUA_PRINTF_FORMAT(fmt, va) \
  __attribute__((__format__(__printf__, fmt, va)))
# define NCLUA_UNUSED __attribute__((__unused__))
#else
# define NCLUA_NORETURN
# define NCLUA_PRINTF_FORMAT
# define NCLUA_UNUSED
#endif

#if NCLUA_GNUC_PREREQ (2,96)
# define NCLUA_LIKELY(expr)   __builtin_expect ((expr), 1)
# define NCLUA_UNLIKELY(expr) __builtin_expect ((expr), 0)
#else
# define NCLUA_LIKELY(expr)   (expr)
# define NCLUA_UNLIKELY(expr) (expr)
#endif

#if NCLUA_GNUC_PREREQ (3,0)
# define NCLUA_ALWAYS_INLINE inline __attribute__((__always_inline__))
# define NCLUA_CONST  __attribute__((__const__))
# define NCLUA_MALLOC __attribute__((__malloc__))
# define NCLUA_PURE   __attribute__((__pure__))
#else
# define NCLUA_ALWAYS_INLINE inline
# define NCLUA_CONST
# define NCLUA_MALLOC
# define NCLUA_PURE
#endif

#if NCLUA_GNUC_PREREQ (3,3)
# define NCLUA_DEPRECATED    __attribute__((__deprecated__))
# define NCLUA_NONNULL(args) __attribute__((__nonnull__ args))
#else
# define NCLUA_DEPRECATED
# define NCLUA_NONNULL(args)
#endif

#if NCLUA_GNUC_PREREQ (3,4)
# define NCLUA_ARG_UNUSED(arg) arg NCLUA_UNUSED
# define NCLUA_USE_RESULT __attribute__((__warn_unused_result__))
#else
# define NCLUA_ARG_UNUSED(arg)
# define NCLUA_USE_RESULT
#endif

#if NCLUA_GNUC_PREREQ (3,3) \
  && (defined (__ELF__) || defined (__APPLE__)) && !defined (__sun)
# define NCLUA_PRIVATE extern __attribute__((__visibility__("hidden")))
#elif defined (__SUNPRO_C) && (__SUNPRO_C >= 0x550)
# define NCLUA_PRIVATE extern __hidden
#else
# define NCLUA_PRIVATE extern
#endif

#if defined (_MSC_VER) && !defined (__cplusplus)
# undef  inline
# define inline __inline
#endif

#ifdef __STRICT_ANSI__
# undef  inline
# define inline __inline__
#endif

#endif /* NCLUA_COMPILER_PRIVATE_H */
