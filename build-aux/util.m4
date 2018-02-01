dnl util.m4 -- Utility macros for Autoconf.
dnl Copyright (C) 2015-2018 Free Software Foundation, Inc.
dnl
dnl This program is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This program is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with this program.  If not, see <https://www.gnu.org/licenses/>.

# AU_ARG_ENABLE(NAME, DESCRIPTION, [DEFAULT=no])
# ----------------------------------------------
# Adds option --enable-[NAME], with description DESCRIPTION and default
# value DEFAULT (either 'yes' or 'no') to the output configure script.
#
# Defines the shell variable enable_[NAME].
# Defines the conditional ENABLE_[NAME] (in uppercase).
#
AC_DEFUN([AU_ARG_ENABLE],[dnl
AC_ARG_ENABLE(AS_TR_SH([$1]),
 [AS_HELP_STRING([--enable-$1],
   [$2 @<:@default=]m4_default([$3], [no])[@:>@])],
 [AS_CASE([$enableval], [yes|no], [:],
   [AC_MSG_ERROR([bad value '$enableval' for --enable-$1 option])])
  AS_TR_SH([enable_$1])=$enableval],
 [AS_TR_SH([enable_$1])=m4_default([$3],[no])])
AM_CONDITIONAL([ENABLE_]m4_toupper(AS_TR_SH([$1])),
 [test "][$][AS_TR_SH([enable_$1])][" = yes])])

# AU_ARG_ENABLE_ANSI
# ------------------
# Adds -ansi and -pedantic to CFLAGS.
#
# Defines the conditional ENABLE_ANSI.
#
AC_DEFUN_ONCE([AU_ARG_ENABLE_ANSI],[dnl
AU_ARG_ENABLE([ansi], [enable strict ansi])
AS_IF([test "$enable_ansi" = yes],
 [AS_CASE([" $CFLAGS "], [*[[\ \	]]-ansi[[\ \	]]*], [:],
   [AC_LIBTOOL_COMPILER_OPTION([if $compiler supports -ansi],
     [au_cv_prog_compiler_ansi],
     [-ansi -c conftest.$ac_ext], [],
     [CFLAGS="$CFLAGS -ansi"])])
  AS_CASE([" $CFLAGS "], [*[[\ \	]]-pedantic[[\ \	]]*], [:],
   [AC_LIBTOOL_COMPILER_OPTION([if $compiler supports -pedantic],
     [au_cv_prog_compiler_pedantic],
     [-pedantic -c conftest.$ac_ext], [],
     [CFLAGS="$CFLAGS -pedantic"])])])])

# AU_ARG_ENABLE_COVERAGE
# ----------------------
# Generates code coverage reports using gcov and lcov.
#
# Substitutes the variables:
# - LCOV       path to lcov
# - GENHTML    path to genhtml
#
# Defines the conditional ENABLE_COVERAGE.
#
AC_DEFUN([AU_ARG_ENABLE_COVERAGE],[dnl
AU_ARG_ENABLE([coverage], [enable coverage testing with gcov])
AM_CONDITIONAL([ENABLE_COVERAGE], [test "$enable_coverage" = yes])
AS_IF([test "$enable_coverage" = yes],
 [AC_REQUIRE_AUX_FILE([Makefile.am.coverage])
  AS_IF([test "$enable_debug" = no],
   [AC_MSG_ERROR([--enable-coverage requires --enable-debug])])
  AS_IF([test "$GCC" != yes],
   [AC_MSG_ERROR([--enable-coverage requires GCC])])
  AC_CHECK_PROGS([SHTOOL], [shtool])
  AS_IF([test -n "$SHTOOL"],
   [AS_CASE([`$SHTOOL path $CC`], [*ccache*],
     [AC_MSG_ERROR([#
ccache must be disabled if --enable-coverage is used
You can disable ccache by setting environment variable CCACHE_DISABLE=1])])
   ])
  AC_CHECK_PROGS([LCOV], [lcov])
  AS_IF([test -z "$LCOV"],
   [AC_MSG_ERROR([--enable-coverage requires lcov])])
  AC_CHECK_PROGS([GENHTML], [genhtml])
  AS_IF([test -z "$GENHTML"],
   [AC_MSG_ERROR([--enable-coverage requires genhtml])])
  CFLAGS="$CFLAGS -fprofile-arcs -ftest-coverage"
  CXXFLAGS="$CXXFLAGS -fprofile-arcs -ftest-coverage"
  LDFLAGS="$LDFLAGS -lgcov"])])

# AU_ARG_ENABLE_DEBUG
# -------------------
# Adds debugging flags to CFLAGS.
#
# Defines the conditional ENABLE_DEBUG.
#
AC_DEFUN_ONCE([AU_ARG_ENABLE_DEBUG],[dnl
AU_ARG_ENABLE([debug], [build for debugging])
AS_IF([test "$enable_debug" = yes],
 [AC_DEFINE([DEBUG], [1], [Define to 1 to enable run-time debugging])
  AS_IF([test "$GCC" = yes],
   [AS_CASE([" $CFLAGS "],
     [*[[\ \	]]-O*],
       [CFLAGS=`echo $CFLAGS | $SED 's/-O[[^ ]]* / /;s/-O[[^ ]]*$//'`])
    AS_CASE([" $CFLAGS "], [*[[\ \	]]-g*], [:],
     [AC_LIBTOOL_COMPILER_OPTION([if $compiler accepts -ggdb3],
       [au_cv_prog_compiler_ggdb3],
       [-ggdb3 -c conftest.$ac_ext], [],
       [CFLAGS="$CFLAGS -ggdb3"],
       [CFLAGS="$CFLAGS -g"])])
   AS_CASE([" $CXXFLAGS "],
    [*[[\ \	]]-O*],
      [CXXFLAGS=`echo $CXXFLAGS | $SED 's/-O[[^ ]]* / /;s/-O[[^ ]]*$//'`])],
   [AS_CASE([" $CFLAGS "], [*[[\ \	]]-g*], [:],
     [CFLAGS="$CFLAGS -g"])
    AS_CASE([" $CXXFLAGS "], [*[[\ \	]]-g*], [:],
     [CXXFLAGS="$CXXFLAGS -g"])])])])

# AU_ARG_ENABLE_VALGRIND
# ----------------------
# Runs self tests under valgrind.
#
# Substitutes the variable VALGRIND to the path of valgrind.
# Defines the conditional ENABLE_VALGRIND.
#
AC_DEFUN_ONCE([AU_ARG_ENABLE_VALGRIND],[dnl
AU_ARG_ENABLE([valgrind], [run self tests under valgrind])
AS_IF([test "$enable_valgrind" = yes],
 [AC_CHECK_PROGS([VALGRIND], [valgrind])
  AS_IF([test -n "$VALGRIND"],
   [VALGRIND="$VALGRIND -q --error-exitcode=1 --leak-check=full"],
   [enable_valgrind=no])
  AC_SUBST([VALGRIND])
  AC_MSG_CHECKING([whether self tests are run under valgrind])
  AC_MSG_RESULT([$enable_valgrind])])])

# AU_ARG_ENABLE_WARNINGS([EXCLUDE-LIST])
# --------------------------------------
# Enables enables all warning flags except for those in EXCLUDE-LIST.
#
# Substitutes the variables:
# - WERROR_CFLAGS    compiler error flags
# - WARN_CFLAGS      compiler warning flags
#
# Defines the conditional ENABLE_WARNINGS.
#
# WARNING: Depends on Gnulib's manywarnings.m4 and warnings.m4.
#
AC_DEFUN([AU_ARG_ENABLE_WARNINGS],[dnl
AU_ARG_ENABLE([warnings], [enable lots of compiler warnigns])
AS_IF([test "$enable_warnings" = yes],
 [AC_REQUIRE_AUX_FILE([manywarnings.m4])
  AC_REQUIRE_AUX_FILE([warnings.m4])
  gl_WARN_ADD([-Werror], [WERROR_CFLAGS])
  AC_SUBST([WERROR_CFLAGS])
  gl_MANYWARN_ALL_GCC([ws])
  ws="$ws -Wc++-compat"
  ws="$ws -Wcast-qual"
  ws="$ws -Wconversion"
  ws="$ws -Wdeclaration-after-statement"
  ws="$ws -Wfloat-equal"
  ws="$ws -Wredundant-decls"
  ws="$ws -Wsign-compare"
  ws="$ws -Wsign-conversion"
  ws="$ws -Wundef"
  gl_MANYWARN_COMPLEMENT([ws], [$ws], [$1])
  for w in $ws; do
    gl_WARN_ADD([$w])
  done
  gl_WARN_ADD([-fdiagnostics-show-option])
  gl_WARN_ADD([-funit-at-a-time])
  AC_SUBST([WARN_CFLAGS])
  AC_DEFINE([lint], [1], [Define to 1 if compiler is checking for lint.])
  AH_VERBATIM([FORTIFY_SOURCE],[
/* Enable compile-time and run-time bounds-checking, and some warnings,
   without upsetting glibc 2.15+.  */
#if !defined _FORTIFY_SOURCE && defined __OPTIMIZE__ && __OPTIMIZE__
# define _FORTIFY_SOURCE 2
#endif
   ])])])

# AU_ARG_WITH(NAME, DESCRIPTION, [DEFAULT=check])
# -----------------------------------------------
# Adds option --with-[NAME], with description DESCRIPTION and default
# value DEFAULT ('yes', 'no', or 'check') to the output configure script.
#
# Defines the shell variable with_[NAME].
#
AC_DEFUN([AU_ARG_WITH],[dnl
AC_ARG_WITH(AS_TR_SH([$1]),
 [AS_HELP_STRING([--with-$1],
   [$2 @<:@default=]m4_default([$3], [check])[@:>@])],
 [AS_CASE([$]AS_TR_SH([with_$1]), [yes|no|check], [:],
   [AC_MSG_ERROR([bad value '][$]AS_TR_SH([with_$1])[' for --with-$1 option])])],
 [AS_TR_SH([with_$1])=check])])

# AU_CHECK_LUA([MIN-VERSION], [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# ---------------------------------------------------------------------
# Checks if Lua >= MIN-VERSION (defaults to 5.1) exists.
#
# If successful, substitutes the variables:
# - LUA_VERSION    Lua version
# - LUA_CFLAGS     Lua CFLAGS
# - LUA_LIBS       Lua LIBS
# And runs ACTION-IF-FOUND (default: no-op).
# Otherwise, runs ACTION-IF-NOT-FOUND (default: exits in error).
#
# Adds the configure options:
# --with-lua-pc-name=NAME    Lua pkg-config module name
# --with-luadatadir=DIR      Lua datadir path
# --with-lualibdir=DIR       Lua libdir path
#
AC_DEFUN_ONCE([AU_CHECK_LUA],[dnl
dnl Install configure options.
AC_ARG_WITH([lua_pc],
 [AS_HELP_STRING([--with-lua-pc=NAME],
   [use NAME as the pkg-config module name for Lua])])
AC_ARG_WITH([luadatadir],
 [AS_HELP_STRING([--with-luadatadir=DIR],
   [install Lua scripts into DIR])])
AC_ARG_WITH([lualibdir],
 [AS_HELP_STRING([--with-lualibdir=DIR],
   [install Lua C modules into DIR])])
dnl Check the given minimal version.
AS_CASE("m4_default([$1], [5.1])",
 [5.1], [au_lua_min_version_minor=1],
 [5.2], [au_lua_min_version_minor=2],
 [5.3], [au_lua_min_version_minor=3],
        [AC_MSG_ERROR([invalid Lua version '$1'])])
au_lua_min_version="5.$au_lua_min_version_minor"
au_lua_min_version_num="50$au_lua_min_version_minor"
AC_DEFINE_UNQUOTED([LUA_REQUIRED_VERSION],
 [$au_lua_min_version_num],
 [Lua required version.])
dnl Check if we can use pkg-config.
AC_REQUIRE([PKG_PROG_PKG_CONFIG])
au_lua_pc="$with_lua_pc"
AS_IF([test -n "$au_lua_pc"],
 [PKG_CHECK_EXISTS([$au_lua_pc >= $au_lua_min_version], [:],
   [AS_UNSET([au_lua_pc])])])
AS_IF([test -z "$au_lua_pc"],
 [PKG_CHECK_EXISTS([lua >= $au_lua_min_version], [au_lua_pc=lua], [:])
  AS_IF([test -z "$au_lua_pc"],
   [for au_lua_min in `seq 3 -1 $au_lua_min_version_minor`; do
      for au_lua_prefix in lua5 lua5. lua-5 lua-5.; do
        PKG_CHECK_EXISTS(
         [${au_lua_prefix}${au_lua_min} >= $au_lua_min_version],
         [au_lua_pc="${au_lua_prefix}${au_lua_min}"], [:])
        AS_IF([test -n "$au_lua_pc"], [break])
      done
      AS_IF([test -n "$au_lua_pc"], [break])
    done])])
AS_IF([test -n "$au_lua_pc"],
dnl Try pkg-config.
 [AC_REQUIRE([PKG_PROG_PKG_CONFIG])
  AU_CHECK_PKG([LUA], [$au_lua_pc >= $au_lua_min_version],
   [AC_LANG_PROGRAM([[
#include <lua.h>
#if LUA_REQUIRED_VERSION_NUM > LUA_VERSION_NUM
# error "lua is too old"
#endif
   ]])],
   [AC_LANG_PROGRAM([[]], [[lua_pushnil (0);]])],
   [au_check_lua_continue=yes],
   [au_check_lua_continue=no])],
dnl Try the standard '-llua'.
 [AC_MSG_CHECKING([LUA])
  AC_REQUIRE([AC_CHECK_LIBM])
  au_check_lua_continue=no
  LUA_CFLAGS=""
  LUA_LIBS="-llua $LIBS"
  AU_LANG_C([$LUA_CFLAGS], [$LUA_CFLAGS], [$LUA_LIBS],
   [AC_COMPILE_IFELSE(
     [AC_LANG_PROGRAM([[
#include <lua.h>
#if LUA_REQUIRED_VERSION_NUM > LUA_VERSION_NUM
# error "lua is too old"
#endif
     ]])],
     [AC_LINK_IFELSE(
       [AC_LANG_PROGRAM([[]], [[lua_pushnil (0);]])],
       [au_check_lua_continue=yes])])])
  AS_IF([test "$au_check_lua_continue" = yes],
   [AC_SUBST([LUA_CFLAGS])
    AC_SUBST([LUA_LIBS])])
  AC_MSG_RESULT([$au_check_lua_continue])])
dnl Try to find the actual version of the library we are using.
AS_IF([test "$au_check_lua_continue" = yes],
 [AC_MSG_CHECKING([the exact version of the lua we're using])
  AS_UNSET([au_lua_version])
  AU_LANG_C([$LUA_CFLAGS], [$LUA_CFLAGS], [$LUA_LIBS],
   [AC_RUN_IFELSE(
     [AC_LANG_PROGRAM([[
#include <lua.h>
     ]],
     [[return !(((int) *lua_version (0)) == 501);]])],
     [au_lua_version=5.1],
     [AC_RUN_IFELSE(
       [AC_LANG_PROGRAM([[
#include <lua.h>
       ]],
       [[return !(((int) *lua_version (0)) == 502);]])],
       [au_lua_version=5.2],
       [AC_RUN_IFELSE(
         [AC_LANG_PROGRAM([[
#include <lua.h>
         ]],
         [[return !(((int) *lua_version (0)) == 503);]])],
         [au_lua_version=5.3])])])])
  AS_IF([test -n "$au_lua_version"],
   [AC_MSG_RESULT([$au_lua_version])],
   [au_lua_version=$au_lua_min_version
    AC_MSG_RESULT([$au_lua_version?])])])
dnl Epilogue.
AC_SUBST([LUA_VERSION], [$au_lua_version])
luadatadir="$with_luadatadir"
AS_IF([test -z "$luadatadir"],
 [luadatadir='${datadir}'/lua/$LUA_VERSION])
AC_SUBST([luadatadir])
lualibdir="$with_lualibdir"
AS_IF([test -z "$lualibdir"],
 [lualibdir='${libdir}'/lua/$LUA_VERSION])
AC_SUBST([lualibdir])
AS_IF([test "$au_check_lua_continue" = yes], [$2],
 [m4_default([$3], [AU_ERROR_REQ([lua >= $au_lua_min_version])])])])

# AU_CHECK_OPTIONAL_PKG(NAME, DESCRIPTION, [DEFAULT=check],
#                       PREFIX, MODULES,
#                      [TRY-COMPILE], [TRY-LINK],
#                      [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------------
# Checks if optional pkg-config modules MODULES exist.
#
# If TRY-COMPILE program is given, try to compile it using [PREFIX]_CFLAGS.
# If TRY-LINK program is given, try to link it using [PREFIX]_CFLAGS.
#
# If --with-NAME was given and the modules exist, substitutes the variables:
# - [PREFIX]_CFLAGS    modules cflags
# - [PREFIX]_LIBS      modules libs
# And runs ACTION-IF-FOUND (default: no-op).
# Otherwise, runs ACTION-IF-NOT-FOUND (default: exits in error).
#
# Defines the pre-processor macros and conditionals (in uppercase):
# - WITH_[NAME]
# - WITH_[PREFIX]
#
# Adds the configure options:
# --with-NAME    Check if MODULES exist.
#
AC_DEFUN([AU_CHECK_OPTIONAL_PKG],[dnl
AU_ARG_WITH([$1], [$2], [$3])
au_check_optional_pkg_$4=no
AS_IF([test "][$][AS_TR_SH([with_$1])][" != no],
 [AU_CHECK_PKG([$4], [$5], [$6], [$7],
   [au_check_optional_pkg_$4=yes],
   [au_check_optional_pkg_$4=no])
  AS_IF([test "][$][AS_TR_SH([with_$1])][" = yes],
   [AS_IF([test "$au_check_optional_pkg_$4" = no],
     [m4_default([$9], [_AU_ERROR_REQ([--with-$1 requires ], [$5])])],
     [$8])])])
AC_MSG_CHECKING([whether to $2])
AC_MSG_RESULT([$au_check_optional_pkg_$4])
AS_IF([test "$au_check_optional_pkg_$4" = yes],
 [AS_TR_SH([with_$1_result])=yes
  AC_DEFINE([WITH_$4], [1], [Define to 1 if you have ]$5[.])],
 [AS_IF([test "][$][AS_TR_SH([with_$1])][" = check],
   [AS_TR_SH([with_$1_result])='no	(requires: m4_chomp($5))'],
   [AS_TR_SH([with_$1_result])='no'])])
AM_CONDITIONAL([WITH_]m4_toupper(AS_TR_SH([$1])),
 [test "$au_check_optional_pkg_$4" = yes])
AM_CONDITIONAL([WITH_$4],
 [test "$au_check_optional_pkg_$4" = yes])])

# AU_CHECK_PKG(PREFIX, MODULES, [TRY-COMPILE], [TRY-LINK],
#             [ACTION-IF-FOUND], [ACTION-IF-NOT-FOUND])
# --------------------------------------------------------
# Checks if pkg-config modules MODULES exist.
#
# If TRY-COMPILE program is given, try to compile it using [PREFIX]_CFLAGS.
# If TRY-LINK program is given, try to link it using [PREFIX]_CFLAGS.
#
# If the given modules exist, substitutes the variables:
# - [PREFIX]_CFLAGS    modules cflags
# - [PREFIX]_LIBS      modules libs
# And runs ACTION-IF-FOUND (default: no-op).
# Otherwise, runs ACTION-IF-NOT-FOUND (default: exits in error).
#
AC_DEFUN([AU_CHECK_PKG],[dnl
AC_REQUIRE([PKG_PROG_PKG_CONFIG])
PKG_CHECK_MODULES([$1], [m4_chomp($2)],
 [au_check_pkg_continue=yes],
  m4_ifnblank([$5], [au_check_pkg_continue=no]))
AS_IF([test "$au_check_pkg_continue" = no],
 [m4_ifblank([$6], [AU_ERROR_REQ([$2])])])
dnl Try compile.
m4_ifblank([$3], [:],
 [AS_IF([test "$au_check_pkg_continue" = yes],
   [AC_MSG_CHECKING([if we can build a program using $1])
    AU_LANG_C([$][$1_CFLAGS], [$][$1_CFLAGS], [$][$1_LIBS],
     [AC_COMPILE_IFELSE([$3],
     [au_check_pkg_continue=yes],
     [au_check_pkg_continue=no])])
    AC_MSG_RESULT([$au_check_pkg_continue])
    AS_IF([test "$au_check_pkg_continue" = no],
     [m4_ifblank([$6],
       [AC_MSG_FAILURE([fail to compile $1 conftest])])])])])
dnl Try link.
m4_ifblank([$4], [:],
 [AS_IF([test "$au_check_pkg_continue" = yes],
   [AC_MSG_CHECKING([if we can link a program to $1])
    AU_LANG_C([$][$1_CFLAGS], [$][$1_CLFAGS], [$][$1_LIBS],
     [AC_LINK_IFELSE([$4],
       [au_check_pkg_continue=yes],
       [au_check_pkg_continue=no])])
    AC_MSG_RESULT([$au_check_pkg_continue])
    AS_IF([test "$au_check_pkg_ok" = no],
     [m4_ifblank([$6],
       [AC_MSG_FAILURE([fail to link $1 conftest])])])])])
dnl
AS_IF([test "$au_check_pkg_continue" = yes], [$5], [$6])])

# AU_ERROR_REQ(PKG)
# -----------------
# Prints error message 'PKG is required' and exits in error.
#
AC_DEFUN([_AU_ERROR_REQ],[dnl
AC_REQUIRE([AC_PROG_SED])
L=`echo "$2" | $SED 's/\([[^ ]]\+ \+[[=!><]]\+ \+[[^ ]]\+\) \+/\1, /g'`
AC_MSG_FAILURE([$1][$L][$3])])
dnl
AC_DEFUN([AU_ERROR_REQ],[dnl
AS_CASE(["$1"],
 [*\ [[=\!\>\<]]*\ *\ [[=\!\>\<]]*\ *],
   [_AU_ERROR_REQ([], [$1], [ are required])],
 [AC_MSG_FAILURE([$1 is required])])])

# AU_GITLOG_TO_CHANGELOG([START-DATE])
# ------------------------------------
# Generates ChangeLog from Git log.
#
# Substitutes the variables:
# - GITLOG_TO_CHANGELOG    path to gitlog-to-changelog script
# - GITLOG_START_DATE      start date to be passed to the above script
# - GITLOG_FIX             path to git-log-fix script, if any
#
# WARNING: Depends on Gnulib's gitlog-to-changelog.
#
AC_DEFUN_ONCE([AU_GITLOG_TO_CHANGELOG],[dnl
AC_REQUIRE_AUX_FILE([gitlog-to-changelog])
AC_REQUIRE_AUX_FILE([Makefile.am.gitlog])
AC_SUBST([GITLOG_TO_CHANGELOG], [$ac_aux_dir/gitlog-to-changelog])
AC_SUBST([GITLOG_FIX], [$ac_aux_dir/git-log-fix])
AC_SUBST([GITLOG_START_DATE], [$1])])

# AU_GIT_VERSION_GEN(PREFIX)
# --------------------------
# Gets version string via Gnulib's git-version-gen script.
# Defines the macros:
# - [PREFIX]_version_string    expands to version string (the whole thing)
# - [PREFIX]_version_major     expands to major version
# - [PREFIX]_version_minor     expands to minor version number
# - [PREFIX]_version_micro     expands to micro version number
#
# WARNING: Depends on Gnulib's git-version-gen script.
# WARNING: This macro must be called before AC_INIT.
#
AC_DEFUN([_au_version_cmd], [build-aux/git-version-gen .tarball-version])
AC_DEFUN([_au_version_re], [^\([[0-9]]+\)\.\([[0-9]]+\)\.?\([[0-9]]+\)?])
AC_DEFUN([au_version_major], [m4_bregexp([$1], _au_version_re, [\1])])
AC_DEFUN([au_version_minor], [m4_bregexp([$1], _au_version_re, [\2])])
AC_DEFUN([au_version_micro], [m4_bregexp([$1], _au_version_re, [\3])])
AC_DEFUN_ONCE([AU_GIT_VERSION_GEN],[dnl
AC_BEFORE([$0], [AC_INIT])
AC_REQUIRE_AUX_FILE([git-version-gen])
m4_define([$1][_version_string], m4_esyscmd(_au_version_cmd))
m4_define([$1][_version_major],
  m4_default(au_version_major($1_version_string), [0]))
m4_define([$1][_version_minor],
  m4_default(au_version_minor($1_version_string), [0]))
m4_define([$1][_version_micro],
  m4_default(au_version_micro($1_version_string), [0]))])

# AU_LANG_C([CPPFLAGS], [CFLAGS], [LIBS], [TEXT])
# -----------------------------------------------
# Appends the given values to CPPFLAGs, CFLAGS, and LIBS and expands TEXT.
# Then restores CFLAGS and LIBS to their previous values.
#
AC_DEFUN([AU_LANG_C],[dnl
AC_LANG_PUSH([C])
m4_ifnblank([$1], [AU_VAR_PUSH([CPPFLAGS], [$1])])
m4_ifnblank([$2], [AU_VAR_PUSH([CFLAGS], [$2])])
m4_ifnblank([$3], [AU_VAR_PUSH([LIBS], [$3])])
$4
m4_ifnblank([$1], [AU_VAR_POP([CPPFLAGS])])
m4_ifnblank([$1], [AU_VAR_POP([CFLAGS])])
m4_ifnblank([$1], [AU_VAR_POP([LIBS])])
AC_LANG_POP])

# AU_LANG_CPLUSPLUS([CPPFLAGS], [CXXFLAGS], [LIBS], [TEXT])
# ---------------------------------------------------------
# Appends the given values to CPPFLAGS, CXXFLAGS and LIBS and expands TEXT.
# Then restores CXXFLAGS and LIBS to their previous values.
#
AC_DEFUN([AU_LANG_CPLUSPLUS],[dnl
AC_LANG_PUSH([C++])
m4_ifnblank([$1], [AU_VAR_PUSH([CPPFLAGS], [$1])])
m4_ifnblank([$2], [AU_VAR_PUSH([CXXFLAGS], [$2])])
m4_ifnblank([$3], [AU_VAR_PUSH([LIBS], [$3])])
$4
m4_ifnblank([$1], [AU_VAR_POP([CPPFLAGS])])
m4_ifnblank([$2], [AU_VAR_POP([CXXFLAGS])])
m4_ifnblank([$3], [AU_VAR_POP([LIBS])])
AC_LANG_POP])

# AU_LIBTOOL_MODULE_LDFLAGS
# -------------------------
# LDFLAGS for library modules.
#
# Substitutes the variable LT_MODULE_LDFLAGS.
#
AC_DEFUN([AU_LIBTOOL_MODULE_LDFLAGS],[dnl
dnl TODO: Check if linker supports these flags.
dnl TODO: Check if we should use -no-undefined on Linux.
LT_MODULE_LDFLAGS="-shared -avoid-version"
AC_SUBST([LT_MODULE_LDFLAGS])])

# AU_LIBTOOL_VERSION(PREFIX, CURRENT, REVISION, AGE)
# --------------------------------------------------
# Defines Libtool version variables for library with the given prefix.
#
# Substitutes the variables:
# - [PREFIX]_LIBTOOL_CURRENT    set to CURRENT
# - [PREFIX]_LIBTOOL_REVISION   set to REVISION
# - [PREFIX]_LIBTOOL_AGE        set to AGE
# - [PREFIX]_LIBTOOL_CURRENT_MINUS_AGE    set to CURRENT minus AGE
# - [PREFIX]_LIBTOOL_STRING     set to CURRENT:REVISION:AGE
#
AC_DEFUN([AU_LIBTOOL_VERSION],[dnl
AC_SUBST([$1][_LIBTOOL_CURRENT], [$2])
AC_SUBST([$1][_LIBTOOL_REVISION], [$3])
AC_SUBST([$1][_LIBTOOL_AGE], [$4])
AC_SUBST([$1][_LIBTOOL_CURRENT_MINUS_AGE], m4_eval([$2 - $4]))
AC_SUBST([$1][_LIBTOOL_STRING], [$2:$3:$4])])

# AU_PROG_CC
# ----------
# Checks for a working C compiler.
# If possible, adds -pipe to CFLAGS.
#
AC_DEFUN_ONCE([AU_PROG_CC],[dnl
AC_REQUIRE([AC_PROG_CC])
AC_REQUIRE([AC_PROG_CPP])
AC_REQUIRE([AC_C_CONST])
dnl Use gcc's -pipe option for faster compilation.
AS_CASE([" $CFLAGS "], [*[[\ \	]]-pipe[[\ \	]]*], [:],
 [AC_LIBTOOL_COMPILER_OPTION([if $compiler supports -pipe],
  [au_cv_prog_compiler_pipe],
  [-pipe -c conftest.$ac_ext], [],
  [CFLAGS="$CFLAGS -pipe"])])])

# AU_PROG_CC_VISIBILITY
# ---------------------
# Checks for a C compiler with support to visibility flags.
# Substitutes the variable CFLAG_VISIBILITY to visibility flags, if any.
# Defines the pre-processor macro HAVE_VISIBILITY to 1 if compiler supports
# visibility flags.
#
# Defines the conditional HAVE_VISIBILITY.
#
# WARNING: Depends on Gnulib's visibility.m4.
#
AC_DEFUN_ONCE([AU_PROG_CC_VISIBILITY],[dnl
AC_REQUIRE([AU_PROG_CC])
AC_REQUIRE_AUX_FILE([visibility.m4])
AC_REQUIRE([gl_VISIBILITY])
AM_CONDITIONAL([HAVE_VISIBILITY],
 [test "$HAVE_VISIBILITY" = 1])])

# AU_PROG_PKG_CONFIG
# ------------------
# Checks for pkg-config.  If it is found substitutes the variable PKGCONFIG
# to the path of pkg-config; otherwise exists in error.
#
# WARNING: Depends on macros defined in pkg-config's pkg.m4.
#
AC_DEFUN_ONCE([AU_PROG_PKG_CONFIG],[dnl
AC_REQUIRE([PKG_PROG_PKG_CONFIG])
AS_IF([test -z "$PKG_CONFIG"],
 [AU_ERROR_REQ([pkg-config >= $PKGCONFIG_REQUIRED])])])

# AU_PROG_UNIX_TOOLS
# ------------------
# Checks for common Unix tools.
#
# Substitutes the variables:
# - INSTALL    path to install
# - SED        path to sed
# - M4         path to m4
# - PERL       path to perl
#
# Defines the conditionals HAVE_M4 and HAVE_PERL.
#
# WARNING: Depends on Gnulib's perl.m4.
#
AC_DEFUN_ONCE([AU_PROG_UNIX_TOOLS],[dnl
AC_REQUIRE([AC_PROG_INSTALL])
AC_REQUIRE([AC_PROG_SED])
AC_CHECK_PROGS([M4], [m4 gm4])
AM_CONDITIONAL([HAVE_M4], [test -n "$M4"])
AC_REQUIRE_AUX_FILE([perl.m4])
AC_REQUIRE([gl_PERL])
AM_CONDITIONAL([HAVE_PERL], [test -n "$PERL"])])

# AU_SYSTEM
# ---------
# Common system checks.
#
# Substitutes the variable:
# - SOEXT     extension of shared libraries on this system
#
AC_DEFUN_ONCE([AU_SYSTEM],[dnl
AC_REQUIRE([LT_SYS_MODULE_EXT])
AC_SUBST([SOEXT], [$libltdl_cv_shlibext])])

# AU_SYSTEM_MINGW
# ---------------
# Checks for MinGW.
#
# Substitutes the variables:
# - MINGW_ROOT              MinGW root directory
# - LIB_EXE_MACHINE_FLAG    value for -machine flag of lib.exe
# - WINDRES                 path to windres
# - NM                      path to nm
# - RANLIB                  path to ranlib
#
# Defines the conditionals:
# - OS_WIN32      true if system is MinGW
# - OS_WIN32_X64  true if system is 64bit
# - HAVE_MS_LIB   true if system has the lib.exe tool
#
AC_DEFUN_ONCE([AU_SYSTEM_MINGW],[dnl
AC_MSG_CHECKING([for native Win32])
dnl Check if we're MinGW.
AC_REQUIRE([AC_CANONICAL_HOST])
AS_CASE(["$host"], [*-*-mingw*],
 [au_os_win32=yes],
 [au_os_win32=no])
AC_MSG_RESULT([$au_os_win32])
AM_CONDITIONAL([OS_WIN32], [test "$au_os_win32" = yes])
dnl Find MinGW root.
AS_IF([test "$au_os_win32" = yes && test -z "$MINGW_ROOT"],
 [MINGW_ROOT="$HOMEDRIVE$MINGW_PREFIX"
  AS_IF([test -z "$MINGW_ROOT"],
   [MINGW_ROOT='C:/msys64'])])
AC_SUBST([MINGW_ROOT])
dnl Check if system is 64bit.
AS_IF([test "$au_os_win32" = yes],
 [AS_CASE(["$host"], [x86_64-*-*],
   [LIB_EXE_MACHINE_FLAG=X64],
   [LIB_EXE_MACHINE_FLAG=X86])])
AC_SUBST([LIB_EXE_MACHINE_FLAG])
AM_CONDITIONAL([OS_WIN32_X64], [test "$LIB_EXE_MACHINE_FLAG" = "X64"])
dnl Check for windres, nm, ranlib, and lib.exe.
AS_IF([test "$au_os_win32" = yes],
 [AC_CHECK_TOOL([WINDRES], [windres])
  AS_IF([test -z "$WINDRES"],
   [AU_ERROR_REQ([windres])])
  AC_CHECK_TOOL([NM], [nm])
  AS_IF([test -z "$NM"],
   [AU_ERROR_REQ([nm])])
  AC_CHECK_TOOL([RANLIB], [ranlib], [:])
  AC_CHECK_PROG([ms_lib], [lib.exe], [yes], [no])])
AM_CONDITIONAL([HAVE_MS_LIB], [test "$ms_lib" = "yes"])
dnl Ensure MSVC-compatible struct packing convention is used when compiling
dnl for Win32 with GCC.
AS_IF([test "$au_os_win32" = yes],
 [AS_IF([test "$GCC" = yes],
   [AC_LIBTOOL_COMPILER_OPTION([if $compiler supports -fnative-struct],
     [au_cv_prog_compiler_fnative_struct],
     [-fnative-struct -c conftest.$ac_ext], [],
     [CFLAGS="$CFLAGS -fnative-struct"],
     [AC_LIBTOOL_COMPILER_OPTION([if $compiler supports -mms-bitfields],
       [au_cv_prog_compiler_mms_bitfields],
       [-mms-bitfields -c conftest.$ac_ext], [],
       [CFLAGS="$CFLAGS -mms-bitfields"],
       [AC_MSG_WARN([dnl
produced libraries might be incompatible with MSVC-compiled code])])])])])])

# AU_VAR_PUSH(NAME, [VALUE])
# AU_VAR_POP(NAME)
# --------------------------
# Push: Appends VALUE to variable NAME.
# Pop: Restores the previous value of variable NAME.
#
AC_DEFUN([AU_VAR_PUSH],[dnl
au_saved_$1="[$]$1"
m4_ifnblank([$2], [$1="[$]$1 $2"])])
AC_DEFUN([AU_VAR_POP],[dnl
$1="$au_saved_$1"
AS_UNSET([au_saved_$1])])

# AU_VERSION_BREAK(PREFIX, VERSION-STRING)
# ----------------------------------------
# Breaks VERSION-STRING into major, minor, and micro parts.
#
# Substitutes the variables:
# - [PREFIX]_REQUIRED_MAJOR    major version number
# - [PREFIX]_REQUIRED_MINOR    minor version number
# - [PREFIX]_REQUIRED_MICRO    micro version number
#
m4_define([_AU_VERSION_BREAK],[dnl
AC_DEFINE(m4_toupper([$1])[_REQUIRED_]m4_toupper([$3]),
  m4_default(au_version_$3([$2]), [0]),
 [$1 required $3 version])])
m4_define([AU_VERSION_BREAK],[dnl
_AU_VERSION_BREAK([$1], [$2], [major])
_AU_VERSION_BREAK([$1], [$2], [minor])
_AU_VERSION_BREAK([$1], [$2], [micro])])

# Local Variables:
# mode: autoconf
# End:
