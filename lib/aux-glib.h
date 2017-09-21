/* Copyright (C) 2014-2017 Free Software Foundation, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef AUX_GLIB_H
#define AUX_GLIB_H

#include <glib.h>
#include <glib/gstdio.h>

#define cast(t, x)        ((t)(x))
#define deconst(t, x)     ((t)(ptrdiff_t)(const void *)(x))
#define devolatile(t, x)  ((t)(ptrdiff_t)(volatile void *)(x))
#define integralof(x)     (((char *)(x)) - ((char *) 0))
#define likely            G_LIKELY
#define nelementsof(x)    G_N_ELEMENTS ((x))
#define pointerof(x)      ((void *)((char *) 0 + ((size_t)(x))))
#define strbool(x)        ((x) ? "true" : "false")
#define tryset(p, x)      ((p) ? *(p) = (x) : (x))
#define unlikely          G_UNLIKELY
#define unused(arg)       arg G_GNUC_UNUSED

#ifndef GNUC_PREREQ
# if defined __GNUC__ && defined __GNUC_MINOR__
#  define GNUC_PREREQ(major, minor)\
  ((__GNUC__ > (major)) || ((__GNUC__ == (major)) &&\
                            (__GNUC_MINOR__ >= (minor))))
# else
#  define GNUC_PREREQ(major, minor) 0
# endif
#endif

#if GNUC_PREREQ (4,2)
# define _GCC_PRAGMA(x)   _Pragma (G_STRINGIFY (x))
# define PRAGMA_DIAG(x)   _GCC_PRAGMA (GCC diagnostic x)
#elif defined (__clang__)
# define _CLANG_PRAGMA(x) _Pragma (G_STRINGIFY (x))
# define PRAGMA_DIAG(x)   _CLANG_PRAGMA (clang diagnostic x)
#else
# define PRAGMA_DIAG(x)
#endif

#if GNUC_PREREQ (4,6) || defined (__clang__)
# define PRAGMA_DIAG_PUSH() PRAGMA_DIAG (push)
# define PRAGMA_DIAG_POP()  PRAGMA_DIAG (pop)
#else
# define PRAGMA_DIAG_PUSH()
# define PRAGMA_DIAG_POP()
#endif

#define PRAGMA_DIAG_IGNORE(x)  PRAGMA_DIAG (ignored G_STRINGIFY (x))
#define PRAGMA_DIAG_WARNING(x) PRAGMA_DIAG (warning G_STRINGIFY (x))

#endif /* AUX_GLIB_H */
