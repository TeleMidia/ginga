AC_PREREQ([2.62])
AC_INIT([Ginga],
 [0.13.6],
 [bugs@telemidia.puc-rio.br],
 [ginga],
 [http://www.telemidia.puc-rio.br])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_USE_SYSTEM_EXTENSIONS
AC_CONFIG_SRCDIR([src/ginga.h])
AC_CONFIG_HEADERS([src/config.h])
AH_BOTTOM([#include "ginga.h"])

AM_INIT_AUTOMAKE([1.14 -Wall -Werror gnu subdir-objects])
AM_SILENT_RULES([yes])
AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT([win32-dll])

dnl Ginga version.
AC_DEFINE_UNQUOTED([VERSION], ["$PACKAGE_VERSION"], [Ginga version.])

dnl Data directories.
AC_SUBST([buttondir], ['${pkgdatadir}'/img/button])
AC_SUBST([contextmanagerdir], ['${pkgdatadir}'/contextmanager])
AC_SUBST([fontdir], ['${pkgdatadir}'/font])
AC_SUBST([formatterdir], ['${pkgdatadir}'/cfg/formatter])
AC_SUBST([formatterimgdir], ['${pkgdatadir}'/img/roller])
AC_SUBST([formattermddir], ['${pkgdatadir}'/md])
AC_SUBST([mbdir], ['${pkgdatadir}'/mb/config])
AC_SUBST([tunerdir], ['${pkgdatadir}'/tuner])

dnl Required dependencies version.
m4_define([avcodec_required_version],    [55.39.101])
m4_define([avfilter_required_version],   [3.90.100])
m4_define([avformat_required_version],   [55.19.104])
m4_define([avutil_required_version],     [52.48.101])
m4_define([curl_required_version],       [7.18])
m4_define([glib_required_version],       [2.32.4])
m4_define([nclua_required_version],      [1.0])
m4_define([sdl2_image_require_version],  [2.0])
m4_define([sdl2_required_version],       [2.0])
m4_define([sdl2_ttf_require_version],    [2.0])
m4_define([swresample_required_version], [0.17.104])
m4_define([swscale_required_version],    [2.5.101])
m4_define([xercesc_required_version],    [2.7])
AC_SUBST([AVCODEC_REQUIRED_VERSION],     avcodec_required_version)
AC_SUBST([AVFILTER_REQUIRED_VERSION],    avfilter_required_version)
AC_SUBST([AVFORMAT_REQUIRED_VERSION],    avformat_required_version)
AC_SUBST([AVUTIL_REQUIRED_VERSION],      avutil_required_version)
AC_SUBST([CURL_REQUIRED_VERSION],        curl_required_version)
AC_SUBST([GLIB_REQUIRED_VERSION],        glib_required_version)
AC_SUBST([NCLUA_REQUIRED_VERSION],       nclua_required_version)
AC_SUBST([SDL2_IMAGE_REQUIRE_VERSION],   sdl2_image_require_version)
AC_SUBST([SDL2_REQUIRED_VERSION],        sdl2_required_version)
AC_SUBST([SDL2_TTF_REQUIRED_VERSION],    sdl2_ttf_required_version)
AC_SUBST([SWRESAMPLE_REQUIRED_VERSION],  swresample_required_version)
AC_SUBST([SWSCALE_REQUIRED_VERSION],     swscale_required_version)
AC_SUBST([XERCESC_REQUIRED_VERSION],     xercesc_required_version)

dnl Optional dependencies version.
m4_define([expat_required_version],      [2.1])
m4_define([libbpg_required_version],     [0.9.5])
m4_define([librsvg_required_version],    [2.40.0])
m4_define([libzip_required_version],     [0.11.2])
AC_SUBST([EXPAT_REQUIRED_VERSION],       expat_required_version)
AC_SUBST([LIBBPG_REQUIRED_VERSION],      libbpg_required_version)
AC_SUBST([LIBRSVG_REQUIRED_VERSION],     librsvg_required_version)
AC_SUBST([LIBZIP_REQUIRED_VERSION],      libzip_required_version)

dnl Compiler and tool checks.
AC_PROG_CXX
AC_LANG_CPLUSPLUS
AU_PROG_PKG_CONFIG

dnl Configure options.
AU_ARG_ENABLE_DEBUG
AU_ARG_ENABLE([nclua-trace], [trace NCLua calls], [no])
AS_IF([test "$enable_nclua_trace" = yes],
 [AC_DEFINE([LUAPLAYER_ENABLE_TRACE], [1],
  [Define to 1 to trace NCLua calls.])])

dnl Headers.
AC_CHECK_HEADER([sys/socket.h])
AC_CHECK_HEADER([net/if.h])

dnl Required dependencies.
AU_CHECK_PKG([CURL],
 [libcurl >= curl_required_version])

AU_CHECK_PKG([FFMPEG],
 [libavcodec >= avcodec_required_version
  libavformat >= avformat_required_version
  libavutil >= avutil_required_version
  libavfilter >= avfilter_required_version
  libswresample >= swresample_required_version
  libswscale >= swscale_required_version])

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

AU_CHECK_PKG([NCLUA],
 [nclua >= nclua_required_version])

AX_PTHREAD([],
 [AU_ERROR_REQ([pthreads])])

AU_CHECK_PKG([SDL2],
 [sdl2 >= sdl2_required_version])
AU_CHECK_PKG([SDL2IMAGE],
 [SDL2_image >= sdl2_image_require_version])
AU_CHECK_PKG([SDL2TTF],
 [SDL2_ttf >= sdl2_ttf_require_version])

AU_CHECK_PKG([XERCESC],
 [xerces-c >= xercesc_required_version])

dnl Optional dependencies.
AU_CHECK_OPTIONAL_PKG([isdbt], [build isdbt support], [],
 [ISDBT], [expat >= expat_required_version])

AU_CHECK_OPTIONAL_PKG([multidevice], [build multidevice support], [],
 [MULTIDEVICE], [libjpeg libzip])

AU_CHECK_OPTIONAL_PKG([libbpg], [build with libbpg BPG support], [],
 [LIBBPG], [libbpg >= libbpg_required_version])

AU_CHECK_OPTIONAL_PKG([librsvg], [build with LibRsvg SVG support], [],
 [LIBRSVG], [librsvg-2.0 >= librsvg_required_version])

dnl Optional dependencies that do not use pkg-config.
dnl   $1: name;
dnl   $2: description;
dnl   $3: prefix;
dnl   $4: requires message;
dnl   $5: check (must set "with_optional_$1" to yes if successful)
dnl
AC_DEFUN([_MY_CHECK_OPTIONAL],[dnl
AU_ARG_WITH([$1], [$2])
with_optional_$1=no
AS_IF([test "$with_$1" != no],
 [$5
  AS_IF([test "$with_optional_$1" = no],
   [AS_IF([test "$with_$1" = yes],
    [_AU_ERROR_REQ([--with-$1 requires $4])])])])
AS_IF([test "$with_optional_$1" = yes],
 [with_$1_result=yes
  AC_DEFINE([WITH_$3], [1], [Define to 1 if you have ]$4[.])],
 [AS_IF([test "$with_$1" = check],
  [with_$1_result='no	(requires: $4)'],
  [with_$1_result='no'])
  AC_DEFINE([WITH_$3], [0], [Define to 1 if you have ]$4[.])])
AM_CONDITIONAL([WITH_$3],
 [test "$with_optional_$1" = yes])])

_MY_CHECK_OPTIONAL([berkelium], [build with Berkelium XHTML support],
 [BERKELIUM], [berkelium],
 [AC_HAVE_LIBRARY([berkelium],
  [AC_CHECK_HEADER([berkelium/Berkelium.hpp],
   [with_optional_berkelium=yes])])])

_MY_CHECK_OPTIONAL([espeak], [build with Espeak ssml support],
 [ESPEAK], [libespeak],
 [AC_HAVE_LIBRARY([espeak],
  [AC_CHECK_HEADER([espeak/speak_lib.h],
   [with_optional_espeak=yes; ESPEAK_LIBS=-lespeak])])])
AC_SUBST([ESPEAK_LIBS])

_MY_CHECK_OPTIONAL([linuxdvb], [build linuxdvb support],
 [LINUXDVB], [linux dvb],
 [AC_CHECK_HEADER([linux/dvb/frontend.h],
  [with_optional_linuxdvb=yes])])

dnl Epilogue.
AC_CONFIG_FILES([
Makefile
data/Makefile
src/Makefile
])

AC_OUTPUT
AC_MSG_NOTICE([summary of main build options:
  version:            ${VERSION}
  host type:          ${host}
  install prefix:     ${prefix}
  compiler:           ${CXX}
  cppflags:           ${CPPFLAGS}
  cxxflags:           ${CXXFLAGS}
  ldflags:            ${LDFLAGS}

  with berkelium:     ${with_berkelium_result}
  with espeak:        ${with_espeak_result}
  with libbpg:        ${with_libbpg_result}
  with librsvg:       ${with_librsvg_result}

  with isdbt:         ${with_isdbt_result}
  with linuxdvb:      ${with_linuxdvb_result}
  with multidevice:   ${with_multidevice_result}
])
