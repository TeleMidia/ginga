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
AC_CONFIG_SRCDIR([lib/ginga.h])
AC_CONFIG_HEADERS([lib/config.h])

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
m4_define([pango_required_version],      [1.30.0])
m4_define([soup_required_version],       [2.42])
AC_SUBST([CAIRO_REQUIRED_VERSION],       cairo_required_version)
AC_SUBST([GLIB_REQUIRED_VERSION],        glib_required_version)
AC_SUBST([GSTREAMER_REQUIRED_VERSION],   gstreamer_required_version)
AC_SUBST([GTK_REQUIRED_VERSION],         gtk_required_version)
AC_SUBST([LIBXML_REQUIRED_VERSION],      libxml_required_version)
AC_SUBST([PANGO_REQUIRED_VERSION],       pango_required_version)
AC_SUBST([SOUP_REQUIRED_VERSION],        soup_required_version)

# Optional dependencies version.
m4_define([cef_required_version],        [3.0])
m4_define([librsvg_required_version],    [2.40.0])
m4_define([lua_required_version],        [5.2])
m4_define([nclua_required_version],      [1.0])
m4_define([opengl_required_version],     [17.0])
m4_define([soup_required_version],       [2.42])
AC_SUBST([CEF_REQUIRED_VERSION],         cef_required_version)
AC_SUBST([LIBRSVG_REQUIRED_VERSION],     librsvg_required_version)
AC_SUBST([LUA_REQUIRED_VERSION],         cef_required_version)
AC_SUBST([NCLUA_REQUIRED_VERSION],       nclua_required_version)
AC_SUBST([OPENGL_REQUIRED_VERSION],      opengl_required_version)

# Compiler checks.
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AX_CXX_COMPILE_STDCXX_11([ext],[mandatory])

# Tool checks.
AU_PROG_PKG_CONFIG
AU_PROG_UNIX_TOOLS

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

AU_CHECK_PKG([PANGO],
 [pango >= pango_required_version
  pangocairo >= pango_required_version])

# Optional dependencies.

# AU_CHECK_OPTIONAL_PKG([SOUP], [libsoup-2.4 >= soup_required_version],
#  [AC_LANG_PROGRAM([[
# #include <libsoup/soup.h>
# #if !SOUP_CHECK_VERSION  \
#     (SOUP_REQUIRED_MAJOR,\
#      SOUP_REQUIRED_MINOR,\
#      SOUP_REQUIRED_MICRO)
# # error "soup is too old"
# #endif
#  ]])],
#  [AC_LANG_PROGRAM([[]], [[glib_check_version (0, 0, 0);]])])

AU_CHECK_OPTIONAL_PKG([soup], [build with gingagui GTK support], [],
 [SOUP], libsoup-2.4 >= soup_required_version,
 [AC_LANG_PROGRAM([[
#include <libsoup/soup.h>
#if !SOUP_CHECK_VERSION  \
    (SOUP_REQUIRED_MAJOR,\
     SOUP_REQUIRED_MINOR,\
     SOUP_REQUIRED_MICRO)
# error "soup is too old"
#endif
 ]])],
 [AC_LANG_PROGRAM([[]], [[soup_session_new_with_options (0, 0, 0);]])])

AC_ARG_WITH([cef],
 [AS_HELP_STRING([--with-cef=DIR],
   [build with Chromium embedded at DIR])])
AS_IF([test -n "$with_cef"],
 [cef_dir=$with_cef
  AS_IF([test -d "$with_cef"], [],
   [AC_MSG_FAILURE([--with-cef was given a bad directory: $with_cef])])
  with_cef=yes
  with_cef_result="yes	($cef_dir)"
  CEF_CFLAGS="-I$cef_dir -I$cef_dir/include"
  CEF_LIBS="-L$cef_dir/Release -lcef"
  CEF_LIBS="$CEF_LIBS $cef_dir/libcef_dll_wrapper/libcef_dll_wrapper.a"
  AU_LANG_CPLUSPLUS([$CEF_CFLAGS], [$CEF_CFLAGS], [$CEF_LIBS],
   [AC_CHECK_HEADER([cef_app.h], [],
     [AC_MSG_FAILURE([cef_app.h is missing])])
    AC_LINK_IFELSE(
     [AC_LANG_PROGRAM([[#include <cef_app.h>]],
       [[CefMainArgs args (0, NULL);]])],
     [],
     [AC_MSG_FAILURE([cannot link to libcef])])])
  AC_SUBST([CEF_CFLAGS])
  AC_SUBST([CEF_LIBS])
  AC_DEFINE([WITH_CEF], [1], [Define to 1 if you have libcef.])],
 [cef_dir=""
  with_cef=no
  with_cef_result='no	(requires: cef)'
  AC_DEFINE([WITH_CEF], [0], [Define to 1 if you have libcef.])])
AC_SUBST([cef_dir])
AM_CONDITIONAL([WITH_CEF], [test "$with_cef" = yes])

AU_CHECK_OPTIONAL_PKG([librsvg], [build with LibRsvg support], [],
 [LIBRSVG],
 [librsvg-2.0 >= librsvg_required_version])

AU_CHECK_LUA([lua_required_version], [with_lua=yes], [with_lua=no])
AS_IF([test "$au_lua_version" = 5.3],
 [AC_DEFINE([LUA_COMPAT_APIINTCASTS], [1],
  [Define to 1 to enable Lua macros for integer manipulation])])
AS_IF([test "$with_lua" = yes],
 [with_lua=yes
  with_lua_result="yes"
  LUA_LIBS="$LUA_LIBS -export-dynamic"
  AC_SUBST([LUA_LIBS])
  AC_DEFINE([WITH_LUA], [1], [Define to 1 if you have Lua.])],
 [with_lua=no
  with_lua_result="no	(requires: lua >= $au_lua_min_version_num)"
  AC_DEFINE([WITH_LUA], [0], [Define to 1 if you have Lua.])])
AM_CONDITIONAL([WITH_LUA], [test "$with_lua" = yes])

AU_CHECK_OPTIONAL_PKG([nclua], [build with NCLua support], [],
 [NCLUA],
 [nclua >= nclua_required_version])

AU_CHECK_OPTIONAL_PKG([opengl], [build with OpenGL support], [],
 [OPENGL],
 [gl >= opengl_required_version
  sdl2])

# Aliases for compiler flags.
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $WARN_CFLAGS $WERROR_CFLAGS "])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $CAIRO_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $CEF_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $GLIB_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $GSTREAMER_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $GTK_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $LIBRSVG_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $LIBXML_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $LUA_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $NCLUA_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $OPENGL_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $PANGO_CFLAGS"])
AS_VAR_APPEND([GINGA_ALL_CXXFLAGS], [" $SOUP_CFLAGS"])
AC_SUBST([GINGA_ALL_CXXFLAGS])

AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $CAIRO_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $CEF_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $GLIB_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $GSTREAMER_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $GTK_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $LIBRSVG_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $LIBXML_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $LUA_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $NCLUA_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $OPENGL_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $PANGO_LIBS"])
AS_VAR_APPEND([GINGA_ALL_LDFLAGS],  [" $SOUP_LIBS"])
AC_SUBST([GINGA_ALL_LDFLAGS])

# Directories.
gingadatadir="$datadir/$PACKAGE"
gingasysconfdir="$sysconfdir/$PACKAGE"
AC_SUBST([gingadatadir])
AC_SUBST([gingasysconfdir])

# Epilogue.
AC_CONFIG_FILES([
Makefile
lib/Makefile
lib/ginga.pc
src/Makefile
src-gui/Makefile
src-gui/icons/Makefile
src-gui/style/Makefile
tests-ncl/Makefile
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

  build html player:     ${with_cef_result}
  build nclua player:    ${with_nclua_result}
  build svg player:      ${with_librsvg_result}
  build ncl-ltab parser: ${with_lua_result}

  Optional executables:
  ginga-gui (gtk):    ${with_soup_result}
  ginga-gl:           ${with_opengl_result}
])
