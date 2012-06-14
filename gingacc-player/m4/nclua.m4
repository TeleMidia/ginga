# nclua.m4 -- Configure macros for NCLua.
# Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

# NCLUA_USE_GCC_PIPE_OPTION
# ----------------------
# Use gcc's -pipe option if available: for faster compilation.

# Adapted from GNU m4 configure.ac code.
dnl Copyright (C) 1991, 1992, 1993, 1994, 2000, 2001, 2002, 2004, 2005,
dnl 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.

AC_DEFUN([NCLUA_USE_GCC_PIPE_OPTION],
[
  case " $CFLAGS " in
    *[\ \	]-pipe[\ \	]*) ;;
    *) _LT_COMPILER_OPTION([if $compiler supports -pipe],
        [nclua_cv_prog_compiler_pipe],
        [-pipe -c conftest.$ac_ext], [],
        [CFLAGS="$CFLAGS -pipe"])
      ;;
  esac
])

# NCLUA_VERSION(PREFIX)
# -----------------
# Get package version number using PACKAGE_VERSION.
# Defines the m4 macro NCLUA_VERSION_PREFIX to the given PREFIX.
# This macro substitutes the following variables:
# PREFIX_VERSION_MAJOR
# PREFIX_VERSION_MINOR
# PREFIX_VERSION_PATCH

AC_DEFUN([NCLUA_VERSION],
[
  AC_REQUIRE([AC_PROG_SED])

  dnl This is used by other NCLUA_* macros.
  m4_define([NCLUA_VERSION_PREFIX], [$1])dnl

m4_changequote(,)dnl
  nclua_major_pattern='s/^\([0-9][0-9]*\)\..*/\1/p'
  nclua_minor_pattern='s/^[0-9][0-9]*\.\([0-9][0-9]*\).*/\1/p'
  nclua_patch_pattern='s/^.*\..*\.\([0-9][0-9]*\).*/\1/p'
m4_changequote([,])dnl

  nclua_major=`echo "$PACKAGE_VERSION" | sed -n -e $nclua_major_pattern`
  nclua_minor=`echo "$PACKAGE_VERSION" | sed -n -e $nclua_minor_pattern`
  nclua_patch=`echo "$PACKAGE_VERSION" | sed -n -e $nclua_patch_pattern`

  if test "x$nclua_major" = x; then
    nclua_major=0
  fi
  if test "x$nclua_minor" = x; then
    nclua_minor=0
  fi
  if test "x$nclua_patch" = x; then
    nclua_patch=0
  fi

  AC_SUBST(NCLUA_VERSION_PREFIX[_VERSION_MAJOR], [$nclua_major])
  AC_SUBST(NCLUA_VERSION_PREFIX[_VERSION_MINOR], [$nclua_minor])
  AC_SUBST(NCLUA_VERSION_PREFIX[_VERSION_PATCH], [$nclua_patch])
  AC_SUBST(NCLUA_VERSION_PREFIX[_VERSION_STRING],
           [\"$nclua_major.$nclua_minor.$nclua_patch\"])
])

# NCLUA_VISIBILITY
# -------------
# Tests whether the compiler supports the command-line option
# -fvisibility=hidden and the function and variable attributes
# __attribute__((__visibility__("hidden"))) and
# __attribute__((__visibility__("default"))).
# Does *not* test for __visibility__("protected") - which has tricky
# semantics (see the 'vismain' test in glibc) and does not exist e.g. on
# MacOS X.
# Does *not* test for __visibility__("internal") - which has processor
# dependent semantics.
# Does *not* test for #pragma GCC visibility push(hidden) - which is
# "really only recommended for legacy code".
# Set the variable CFLAG_VISIBILITY.
# Defines and sets the variable HAVE_VISIBILITY.

# Adapted from Gnulib visibility.m4 (serial 4) code.
dnl Copyright (C) 2005, 2008, 2010-2011 Free Software Foundation, Inc.
dnl From Bruno Haible.

AC_DEFUN([NCLUA_VISIBILITY],
[
  AC_REQUIRE([AC_PROG_CC])
  CFLAG_VISIBILITY=
  HAVE_VISIBILITY=0
  if test -n "$GCC"; then
    dnl First, check whether -Werror can be added to the command line, or
    dnl whether it leads to an error because of some other option that the
    dnl user has put into $CC $CFLAGS $CPPFLAGS.
    AC_MSG_CHECKING([whether the -Werror option is usable])
    AC_CACHE_VAL([nclua_cv_cc_vis_werror], [
      nclua_save_CFLAGS="$CFLAGS"
      CFLAGS="$CFLAGS -Werror"
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM([[]], [[]])],
        [nclua_cv_cc_vis_werror=yes],
        [nclua_cv_cc_vis_werror=no])
      CFLAGS="$nclua_save_CFLAGS"])
    AC_MSG_RESULT([$nclua_cv_cc_vis_werror])
    dnl Now check whether visibility declarations are supported.
    AC_MSG_CHECKING([for simple visibility declarations])
    AC_CACHE_VAL([nclua_cv_cc_visibility], [
      nclua_save_CFLAGS="$CFLAGS"
      CFLAGS="$CFLAGS -fvisibility=hidden"
      dnl We use the option -Werror and a function dummyfunc, because on some
      dnl platforms (Cygwin 1.7) the use of -fvisibility triggers a warning
      dnl "visibility attribute not supported in this configuration; ignored"
      dnl at the first function definition in every compilation unit, and we
      dnl don't want to use the option in this case.
      if test $nclua_cv_cc_vis_werror = yes; then
        CFLAGS="$CFLAGS -Werror"
      fi
      AC_COMPILE_IFELSE(
        [AC_LANG_PROGRAM(
           [[extern __attribute__((__visibility__("hidden"))) int hiddenvar;
             extern __attribute__((__visibility__("default"))) int exportedvar;
             extern __attribute__((__visibility__("hidden"))) int hiddenfunc (void);
             extern __attribute__((__visibility__("default"))) int exportedfunc (void);
             void dummyfunc (void) {}
           ]],
           [[]])],
        [nclua_cv_cc_visibility=yes],
        [nclua_cv_cc_visibility=no])
      CFLAGS="$nclua_save_CFLAGS"])
    AC_MSG_RESULT([$nclua_cv_cc_visibility])
    if test $nclua_cv_cc_visibility = yes; then
      CFLAG_VISIBILITY="-fvisibility=hidden"
      HAVE_VISIBILITY=1
    fi
  fi
  AC_SUBST([CFLAG_VISIBILITY])
  AC_SUBST([HAVE_VISIBILITY])
  AC_DEFINE_UNQUOTED([HAVE_VISIBILITY], [$HAVE_VISIBILITY],
    [Define to 1 or 0, depending whether the compiler supports simple visibility declarations.])
])
