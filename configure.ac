# configure.ac -- Configure template for Ginga.
# Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia
#
# This file is part of Ginga (Ginga-NCL).
#
# Ginga is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# Ginga is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ginga.  If not, see <https://www.gnu.org/licenses/>.

AC_PREREQ([2.62])

AU_GIT_VERSION_GEN([ginga])
AC_INIT([Ginga],
  ginga_version_string,
 [bugs@telemidia.puc-rio.br],
 [ginga],
 [http://github.com/TeleMidia/ginga])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_SRCDIR([src/ginga.cpp])
AC_CONFIG_HEADERS([src/config.h])

AM_INIT_AUTOMAKE([1.14 -Wall -Werror gnu subdir-objects])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT([win32-dll])
AU_LIBTOOL_MODULE_LDFLAGS

# Ginga version.
AC_SUBST([GINGA_VERSION_MAJOR], ginga_version_major)
AC_SUBST([GINGA_VERSION_MINOR], ginga_version_minor)
AC_SUBST([GINGA_VERSION_MICRO], ginga_version_micro)
AC_SUBST([GINGA_VERSION_STRING], ginga_version_string)

# Libginga version.
# - library code modified:            REVISION++
# - interfaces changed/added/removed: REVISION=0, CURRENT++
# - interfaces added:                 AGE++
# - interfaces removed:               AGE=0
AU_LIBTOOL_VERSION([GINGA], [0], [0], [0])

# Required dependencies version.
m4_define([cairo_required_version],      [1.10.2])
m4_define([glib_required_version],       [2.32.4])
m4_define([gstreamer_required_version],  [1.2.0])
m4_define([gtk_required_version],        [3.4.2])
m4_define([libxml_required_version],     [2.9])
m4_define([lua_required_version],        [5.2])
m4_define([nclua_required_version],      [1.0])
m4_define([pango_required_version],      [1.30.0])
AC_SUBST([CAIRO_REQUIRED_VERSION],       cairo_required_version)
AC_SUBST([GLIB_REQUIRED_VERSION],        glib_required_version)
AC_SUBST([GSTREAMER_REQUIRED_VERSION],   gstreamer_required_version)
AC_SUBST([GTK_REQUIRED_VERSION],         gtk_required_version)
AC_SUBST([LIBXML_REQUIRED_VERSION],      libxml_required_version)
AC_SUBST([LUA_REQUIRED_VERSION],         lua_required_version)
AC_SUBST([NCLUA_REQUIRED_VERSION],       nclua_required_version)
AC_SUBST([PANGO_REQUIRED_VERSION],       pango_required_version)

# Compiler checks.
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])

# Tool checks.
AU_PROG_PKG_CONFIG
AU_PROG_UNIX_TOOLS
AC_CHECK_PROGS([XXD], [xxd])
AS_IF([test -z "$XXD"],
 [AU_ERROR_REQ([xxd])])
AM_CONDITIONAL([HAVE_XXD], [test -n "$XXD"])

# System checks.
AU_SYSTEM
AU_SYSTEM_MINGW

# Configure options.
AU_ARG_ENABLE_COVERAGE
AU_ARG_ENABLE_DEBUG
AU_ARG_ENABLE_VALGRIND

nw=
nw="$nw -Wbad-function-cast"             # invalid in C++
nw="$nw -Wc++-compat"                    # invalid in C++
nw="$nw -Wdeclaration-after-statement"   # invalid in C++
nw="$nw -Wdesignated-init"               # invalid in C++
nw="$nw -Wdiscarded-array-qualifiers"    # invalid in C++
nw="$nw -Wdiscarded-qualifiers"          # invalid in C++
nw="$nw -Wduplicate-decl-specifier"      # invalid in C++
nw="$nw -Wimplicit"                      # invalid in C++
nw="$nw -Wimplicit-function-declaration" # invalid in C++
nw="$nw -Wimplicit-int"                  # invalid in C++
nw="$nw -Wincompatible-pointer-types"    # invalid in C++
nw="$nw -Wint-conversion"                # invalid in C++
nw="$nw -Wjump-misses-init"              # invalid in C++
nw="$nw -Wmissing-parameter-type"        # invalid in C++
nw="$nw -Wmissing-prototypes"            # invalid in C++
nw="$nw -Wnested-externs"                # invalid in C++
nw="$nw -Wold-style-declaration"         # invalid in C++
nw="$nw -Wold-style-definition"          # invalid in C++
nw="$nw -Woverride-init"                 # invalid in C++
nw="$nw -Wpointer-sign"                  # invalid in C++
nw="$nw -Wpointer-to-int-cast"           # invalid in C++
nw="$nw -Wstrict-prototypes"             # invalid in C++
nw="$nw -Wfloat-conversion"              # too many warnings
nw="$nw -Wpragmas"                       # too many warnings
nw="$nw -Wshadow"                        # too many warnings
nw="$nw -Wsign-compare"                  # too many warnings
nw="$nw -Wsign-conversion"               # too many warnings
nw="$nw -Wsystem-headers"                # too many warnings
AC_LANG_PUSH([C])
AU_ARG_ENABLE_WARNINGS([$nw])
AC_LANG_POP

# Check for MacOS X
platform_osx=no
AC_MSG_CHECKING([if compiling for Mac OS X])
case "$host_os" in
  darwin*)
     AC_MSG_RESULT(yes)
     AC_DEFINE(PLATFORM_OSX, 1, [define to 1 if compiling for Mac OS X])
     platform_osx=yes
     ;;
  *)
     ;;
esac
AC_MSG_RESULT($platform_osx)
AM_CONDITIONAL(PLATFORM_OSX, test "x$platform_osx" = xyes)

# Required dependencies.
AU_VERSION_BREAK([cairo], cairo_required_version)
AU_CHECK_PKG([CAIRO], [cairo >= cairo_required_version],
 [AC_LANG_PROGRAM([[
#include <cairo.h>
#if CAIRO_VERSION_ENCODE \
   (CAIRO_REQUIRED_MAJOR,\
    CAIRO_REQUIRED_MINOR,\
    CAIRO_REQUIRED_MICRO) > CAIRO_VERSION
# error "cairo is too old"
#endif
 ]])],
 [AC_LANG_PROGRAM([[]], [[cairo_create (0);]])])

AU_VERSION_BREAK([glib], glib_required_version)
AU_CHECK_PKG([GLIB], [glib-2.0 >= glib_required_version],
 [AC_LANG_PROGRAM([[
#include <glib.h>
#if !GLIB_CHECK_VERSION  \
    (GLIB_REQUIRED_MAJOR,\
     GLIB_REQUIRED_MINOR,\
     GLIB_REQUIRED_MICRO)
# error "glib is too old"
#endif
 ]])],
 [AC_LANG_PROGRAM([[]], [[glib_check_version (0, 0, 0);]])])

AU_VERSION_BREAK([gstreamer], gstreamer_required_version)
AU_CHECK_PKG([GSTREAMER],
 [gstreamer-1.0 >= gstreamer_required_version
  gstreamer-video-1.0 >= gstreamer_required_version
  gstreamer-audio-1.0 >= gstreamer_required_version
  gstreamer-app-1.0 >= gstreamer_required_version],
 [AC_LANG_PROGRAM([[
#include <gst/gst.h>
#if !GST_CHECK_VERSION  \
    (GST_REQUIRED_MAJOR,\
     GST_REQUIRED_MINOR,\
     GST_REQUIRED_MICRO)
# error "gstreamer is too old"
#endif
 ]])],
 [AC_LANG_PROGRAM([[]], [[
gst_init (0, 0);
gst_video_info_from_caps (0, 0);
 ]])])

AU_CHECK_PKG([GTK],
 [gtk+-3.0 >= gtk_required_version])

AU_CHECK_PKG([LIBXML],
 [libxml-2.0 >= libxml_required_version])

AU_CHECK_LUA(lua_required_version)
LUA_LIBS="$LUA_LIBS -export-dynamic"
AS_IF([test "$au_lua_version" = 5.3],
 [AC_DEFINE([LUA_COMPAT_APIINTCASTS], [1],
   [Define to 1 to enable Lua macros for integer manipulation])])

AU_CHECK_PKG([PANGO],
 [pango >= pango_required_version
  pangocairo >= pango_required_version])

# Optional dependencies.
AU_CHECK_OPTIONAL_PKG([nclua], [build with NCLua support], [],
 [NCLUA],
 [nclua >= nclua_required_version])

# Directories.
gingadatadir="$datadir/$PACKAGE"
gingasysconfdir="$sysconfdir/$PACKAGE"
gingabindir="$bindir"
AC_SUBST([gingadatadir])
AC_SUBST([gingasysconfdir])
AC_SUBST([gingabindir])

# Epilogue.
AC_CONFIG_FILES([
Makefile
src/Makefile
src/ginga.pc
tests/Makefile
])

AC_OUTPUT
AC_MSG_NOTICE([summary of main build options:
  version:        ${VERSION}
  host:           ${host}
  install prefix: ${prefix}
  compiler:       ${CXX}
  cxxflags:       ${CXXFLAGS}
  ldflags:        ${LDFLAGS}
  warning flags:  ${WERROR_CFLAGS} ${WARN_CFLAGS}
  valgrind:       ${VALGRIND}
])
