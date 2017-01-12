AC_PREREQ([2.62])
AC_INIT([ginga], [0.13.6], [bugs@telemidia.puc-rio.br])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADERS([src/config.h])

dnl required
m4_define([avcodec_required_version],    [55.39.101])
m4_define([avfilter_required_version],   [3.90.100])
m4_define([avformat_required_version],   [55.19.104])
m4_define([avutil_required_version],     [52.48.101])
m4_define([curl_required_version],       [7.18])
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
AC_SUBST([NCLUA_REQUIRED_VERSION],       nclua_required_version)
AC_SUBST([SDL2_IMAGE_REQUIRE_VERSION],   sdl2_image_require_version)
AC_SUBST([SDL2_REQUIRED_VERSION],        sdl2_required_version)
AC_SUBST([SDL2_TTF_REQUIRED_VERSION],    sdl2_ttf_required_version)
AC_SUBST([SWRESAMPLE_REQUIRED_VERSION],  swresample_required_version)
AC_SUBST([SWSCALE_REQUIRED_VERSION],     swscale_required_version)
AC_SUBST([XERCESC_REQUIRED_VERSION],     xercesc_required_version)

dnl optional
m4_define([expat_required_version],      [2.1])
m4_define([libbpg_required_version],     [0.9.5])
m4_define([librsvg_required_version],    [2.40.0])
m4_define([libzip_required_version],     [0.11.2])
AC_SUBST([EXPAT_REQUIRED_VERSION],       expat_required_version)
AC_SUBST([LIBBPG_REQUIRED_VERSION],      libbpg_required_version)
AC_SUBST([LIBRSVG_REQUIRED_VERSION],     librsvg_required_version)
AC_SUBST([LIBZIP_REQUIRED_VERSION],      libzip_required_version)

AM_INIT_AUTOMAKE([-Wall 1.11 foreign gnu no-define subdir-objects])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])
AM_PROG_AR

AC_PROG_CXX
AC_LANG_CPLUSPLUS

LT_PREREQ([2.2])
LT_INIT([disable-static win32-dll])

# Ginga version.
AC_DEFINE_UNQUOTED([VERSION], ["$PACKAGE_VERSION"], [Ginga version.])

# Ginga confdir, libdir, and includedir.
AC_SUBST([gingaconfdir], ['${sysconfdir}'/ginga/files])
AC_SUBST([gingalibdir], ['${libdir}'])
AC_SUBST([gingaincludedir], ['${includedir}'/ginga])

# _AC_ARG_ENABLE(NAME, DESCRIPTION, DEFAULT)
# ------------------------------------------
# Check for argument --enable-NAME and sets $enable_NAME accordingly.
m4_define([_AC_ARG_ENABLE],
 [AC_ARG_ENABLE([$1],
   [AS_HELP_STRING([--enable-$1],
     [$2 [default=$3]])],
   [AS_CASE([$enableval], [yes|no], [:],
     [AC_MSG_ERROR([bad value '$enableval' for --enable-$1 option])])
    enable_[]m4_translit($1,[-],[_])=$enableval],
   [enable_[]m4_translit($1,[-],[_])=$3])])

# _AC_ARG_WITH(NAME, DESCRIPTION, [DEFAULT=check])
# -----------------------------------------------
# Adds option --with-[NAME], with description DESCRIPTION and default
# value DEFAULT ('yes', 'no', or 'check') to the output configure script.
#
# Defines the shell variable with_[NAME].
#
AC_DEFUN([_AC_ARG_WITH],[dnl
AC_ARG_WITH(AS_TR_SH([$1]),
 [AS_HELP_STRING([--with-$1],
   [$2 @<:@default=]m4_default([$3], [check])[@:>@])],
 [AS_CASE([$]AS_TR_SH([with_$1]), [yes|no|check], [:],
   [AC_MSG_ERROR([bad value '][$]AS_TR_SH([with_$1])[' for --with-$1 option])])],
 [AS_TR_SH([with_$1])=check])])

# Check if we're on MS Windows.
case "$target_os" in
  *-*-mingw*)
    os_win=yes
    ;;
  *)
    os_win=no
esac
AM_CONDITIONAL([OS_WINDOWS], [test "$os_win" = yes])

# Socket and sysinfo.
AC_CHECK_HEADER([sys/socket.h])
AC_CHECK_HEADER([net/if.h])
AC_CHECK_HEADER([sys/sysinfo])


# Check for Curl.
AX_LIB_CURL([curl_required_version], [:],
 [AC_MSG_ERROR([
*** CURL >= curl_required_version is required.])])

# Check for FFmpeg libs.
PKG_CHECK_MODULES([FFMPEG],
 [libavcodec >= avcodec_required_version
  libavformat >= avformat_required_version
  libavutil >= avutil_required_version
  libavfilter >= avfilter_required_version
  libswresample >= swresample_required_version
  libswscale >= swscale_required_version], [],
 [AC_MSG_ERROR([
*** The following FFmpeg libraries are required:
*** libavcodec >= avcodec_required_version
*** libavformat >= avformat_required_version
*** libavutil >= avutil_required_version
*** libavfilter >= avfilter_required_version
*** libswresample >= swresample_required_version
*** libswscale >= swscale_required_version])])

# Check for NCLua.
PKG_CHECK_MODULES([NCLUA],
 [nclua >= nclua_required_version], [],
 [AC_MSG_ERROR([
*** nclua >= nclua_required_version is required.])])

# Check for pthreads.
AX_PTHREAD([],
 [AC_MSG_ERROR([
*** pthreads is required.])])

# Check for SDL2.
PKG_CHECK_MODULES([SDL2],
 [sdl2 >= sdl2_required_version], [],
 [AC_MSG_ERROR([
*** sdl2 >= sdl2_required_version is required.])])
AC_DEFINE([HAVE_SDL], [1], [Define to 1 if SDL2 is present.])

# Check for SDL2_image.
PKG_CHECK_MODULES([SDL2IMAGE],
 [SDL2_image >= sdl2_image_require_version], [],
 [AC_MSG_ERROR([
*** SDL2_image >= sdl2_image_required_version is required.])])

# Check for SDL2_ttf.
PKG_CHECK_MODULES([SDL2TTF],
 [SDL2_ttf >= sdl2_ttf_require_version], [],
 [AC_MSG_ERROR([
*** SDL2_ttf >= sdl2_ttf_required_version is required.])])

# Check for X11.
PKG_CHECK_MODULES([X11], [x11], [],
 [AC_MSG_ERROR([
*** X11 is required.])])

# Check for xerces-c.
AX_XERCESC
AS_IF([test "$HAVE_LIBXERCES_C" = no],
 [AC_MSG_ERROR([
*** xerces-c >= xercesc_required_version is required.])])
AC_SUBST([XERCESC_CFLAGS], [])
AC_SUBST([XERCESC_LIBS], [$LIBXERCES_C])


# Option --with-berkelium.
_AC_ARG_WITH([berkelium], [build berkelium xhtml support])
berkelium_ok=no
AC_HAVE_LIBRARY([berkelium],
 [AC_CHECK_HEADER([berkelium/Berkelium.hpp],
  [berkelium_ok=yes])])
AS_IF([test "$with_berkelium" = yes || test "$with_berkelium" = check],
 [AS_IF([test "$berkelium_ok" = yes],
   [with_berkelium=yes],
   [AS_IF([test "$with_berkelium" = yes],
    [AC_MSG_ERROR([
*** Berkelium is required by --with-berkelium.])],
    [with_berkelium="no	(requires Berkelium)"])])])
AS_IF([test "$with_berkelium"],
 [AC_DEFINE([HAVE_BERKELIUM], [1],
  [Define to 1 if berkelium support is enabled.])]
 [AC_DEFINE([HAVE_BERKELIUM], [0],
  [Define to 1 if berkelium support is enabled.])])
AM_CONDITIONAL([BUILD_BERKELIUM],
 [test "with_berkelium" = yes])

# Option --with-espeak.
_AC_ARG_WITH([espeak], [build with espeak ssml support])
espeak_ok=no
AC_HAVE_LIBRARY([espeak],
 [AC_CHECK_HEADER([espeak/speak_lib.h],
  [espeak_ok=yes])])
AS_IF([test "$with_espeak" = yes || test "$with_espeak" = check],
 [AS_IF([test "$espeak_ok" = yes],
  [with_espeak=yes],
  [AS_IF([test "$with_espeak" = yes],
   [AC_MSG_ERROR([
*** Espeak is required by --with-espeak.])],
   [with_espeak="no	(requires Espeak)"])])])
AS_IF([test "$espeak_ok" = yes],
 [ESPEAK_LIBS='-lespeak'])
AC_SUBST([ESPEAK_LIBS])
AS_IF([test "$with_espeak"],
 [AC_DEFINE([HAVE_ESPEAK], [1],
  [Define to 1 if espeak support is enabled.])]
 [AC_DEFINE([HAVE_ESPEAK], [0],
  [Define to 1 if espeak support is enabled.])])
AM_CONDITIONAL([BUILD_ESPEAK],
 [test "with_espeak" = yes])

# Option --with-libbpg.
_AC_ARG_WITH([libbpg], [build with libbpg BPG support])
PKG_CHECK_MODULES([LIBBPG],
 [libbpg >= libbpg_required_version],
 [libbpg_ok=yes],
 [libbpg_ok=no])
AS_IF([test "$with_libbpg" = yes || test "$with_libbpg" = check],
 [AS_IF([test "$libbpg_ok" = yes],
  [with_libbpg=yes],
  [AS_IF([test "$with_libbpg" = yes],
   [AC_MSG_ERROR([
*** Libbpg >= libbpg_required_version is required by --with-libbpg.])],
   [with_libbpg="no	(requires Libbpg >= libbpg_required_version)"])])])
AS_IF([test "$with_libbpg"],
 [AC_DEFINE([HAVE_LIBBPG], [1],
  [Define to 1 if libbpg support is enabled.])]
 [AC_DEFINE([HAVE_LIBBPG], [0],
  [Define to 1 if libbpg support is enabled.])])
AM_CONDITIONAL([BUILD_LIBBPG],
 [test "with_libbpg" = yes])

# Option --with-librsvg.
_AC_ARG_WITH([librsvg], [build with LibRsvg SVG support])
PKG_CHECK_MODULES([LIBRSVG],
 [librsvg-2.0 >= librsvg_required_version],
 [librsvg_ok=yes],
 [librsvg_ok=no])
AS_IF([test "$with_librsvg" = yes || test "$with_librsvg" = check],
 [AS_IF([test "$librsvg_ok" = yes],
   [with_librsvg=yes],
   [AS_IF([test "$with_librsvg" = yes],
    [AC_MSG_ERROR([
*** Librsvg >= librsvg_required_version is required by --with-librsvg.])],
    [with_librsvg="no	(requires Librsvg >= librsvg_required_version)"])])])
AS_IF([test "$with_librsvg"],
 [AC_DEFINE([HAVE_LIBRSVG], [1],
  [Define to 1 if librsvg support is enabled.])]
 [AC_DEFINE([HAVE_LIBRSVG], [0],
  [Define to 1 if librsvg support is enabled.])])
AM_CONDITIONAL([BUILD_LIBRSVG],
 [test "with_librsvg" = yes])

# Option --with-isbdt.
_AC_ARG_WITH([isdbt], [build isdbt support])
AX_LIB_EXPAT([expat_required_version])
AS_IF([test "$with_isdbt" = yes || test "$with_isdbt" = check],
 [AS_IF([test "$HAVE_EXPAT" = yes],
   [with_isdbt=yes],
   [AS_IF([test "$with_isdbt" = yes],
    [AC_MSG_ERROR([
*** Expat >= expat_required_version is required by --with-isdbt.])],
    [with_isdbt="no	(requires Expat >= expat_required_version)"])])])
AS_IF([test "$with_isdbt"],
 [AC_DEFINE([HAVE_ISDBT], [1], [Define to 1 if isdbt support is enabled.])]
 [AC_DEFINE([HAVE_ISDBT], [0], [Define to 1 if isdbt support is enabled.])])
AM_CONDITIONAL([BUILD_ISDBT],
 [test "with_isdbt" = yes])

# Option --with-linuxdvb.
_AC_ARG_WITH([linuxdvb], [build linuxdvb support])
AC_CHECK_HEADER([linux/dvb/frontend.h],
 [linuxdvb_ok=yes],
 [linuxdvb_ok=no])
AS_IF([test "$with_linuxdvb" = yes || test "$with_linuxdvb" = check],
 [AS_IF([test "$linuxdvb_ok" = yes],
  [with_linuxdvb=yes],
  [AS_IF([test "$with_linuxdvb" = yes],
    [AC_MSG_ERROR([
*** Linux DVB headers are required by --with-linuxdvb.])],
    [with_linuxdvb="no	(requires Linux DVB headers)"])])])
AS_IF([test "$with_linuxdvb"],
 [AC_DEFINE([HAVE_LINUXDVB], [1],
  [Define to 1 if linuxdvb support is enabled.])]
 [AC_DEFINE([HAVE_LINUXDVB], [0],
  [Define to 1 if linuxdvb support is enabled.])])
AM_CONDITIONAL([BUILD_LINUXDVB],
 [test "with_linuxdvb" = yes])

# Option --with-multidevice.
_AC_ARG_WITH([multidevice], [build multidevice support])
AC_CHECK_LIB([jpeg], [jpeg_destroy_decompress],
 [jpeg_ok=yes],
 [jpeg_ok=no])
dnl jpeg
AS_IF([test "$jpeg_ok" = yes],
 [AC_MSG_CHECKING([for jpeglib.h])
  AC_TRY_CPP([[
#include <stdio.h>
#undef PACKAGE
#undef VERSION
#undef HAVE_STDLIB_H
#include <jpeglib.h>
  ]],
  [jpeg_ok=yes],
  [jpeg_ok=no])
  AC_MSG_RESULT([$jpeg_ok])
  AS_IF([test "jpeg_ok" = yes],
   [JPEG_LIBS='-ljpeg'])])
AC_SUBST([JPEG_LIBS])
dnl libzip
PKG_CHECK_MODULES([LIBZIP],
  [libzip >= libzip_required_version],
  [have_libzip=yes],
  [have_libzip=no])

AS_IF([test "$with_multidevice" = yes || test "$with_multidevice" = check],
 [AS_IF([test "$jpeg_ok" = yes],
  [AS_IF([test "$have_libzip" = yes],
   [with_multidevice=yes],
   [AS_IF([test "$with_multidevice" = yes],
    [AC_MSG_ERROR([
*** Libzip >= libzip_required_version is required by --with-multidevice.])],
    [with_multidevice="no	(requires Libzip >= libzip_required_version)"])])],
  [AS_IF([test "$with_multidevice" = yes],
   [AC_MSG_ERROR([
*** Libjpeg is required by --with-multidevice.])],
    [with_multidevice="no	(requires Libjpeg)"])])])

AS_IF([test "$with_multidevice"],
 [AC_DEFINE([HAVE_MULTIDEVICE], [1],
  [Define to 1 if multidevice support is enabled.])]
 [AC_DEFINE([HAVE_MULTIDEVICE], [0],
  [Define to 1 if multidevice support is enabled.])])
AM_CONDITIONAL([BUILD_MULTIDEVICE],
 [test "with_multidevice" = yes])


# Switch --enable-nclua-trace.
_AC_ARG_ENABLE([nclua-trace], [trace NCLua calls], [no])
AS_IF([test "$enable_nclua_trace" = yes],
 [AC_DEFINE([LUAPLAYER_ENABLE_TRACE], [1],
  [Define to 1 to trace NCLua calls.])],
 [AC_DEFINE([LUAPLAYER_ENABLE_TRACE], [0],
  [Define to 1 to trace NCLua calls.])])


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
  compiler:           ${CC}
  cppflags:           ${CPPFLAGS}
  cxxflags:           ${CXXFLAGS}
  ldflags:            ${LDFLAGS}

  with berkelium:     ${with_berkelium}
  with espeak:        ${with_espeak}
  with libbpg:        ${with_libbpg}
  with librsvg:       ${with_librsvg}

  with isdbt:         ${with_isdbt}
  with linuxdvb:      ${with_linuxdvb}
  with multidevice:   ${with_multidevice}
  with tests:         ${enable_tests}

  trace nclua calls:  ${enable_nclua_trace}
])
