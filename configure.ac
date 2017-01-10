AC_PREREQ([2.62])
AC_INIT([ginga], [0.13.6], [bugs@telemidia.puc-rio.br])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([build-aux])
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([util/functions.cpp])
AC_CONFIG_HEADERS([config.h])

m4_define([avcodec_required_version],    [55.39.101])
m4_define([avfilter_required_version],   [3.90.100])
m4_define([avformat_required_version],   [55.19.104])
m4_define([avutil_required_version],     [52.48.101])
m4_define([curl_required_version],       [7.18])
m4_define([expat_required_version],      [2.1])
m4_define([libzip_required_version],     [0.11.2])
m4_define([nclua_required_version],      [1.0])
m4_define([sdl2_image_require_version],  [2.0])
m4_define([sdl2_required_version],       [2.0])
m4_define([sdl2_ttf_require_version],    [2.0])
m4_define([swresample_required_version], [0.17.104])
m4_define([swscale_required_version],    [2.5.101])
m4_define([xercesc_required_version],    [2.7])
m4_define([librsvg_required_version],    [2.40.0])
m4_define([libbpg_required_version],     [0.9.5])

AC_SUBST([AVCODEC_REQUIRED_VERSION], avcodec_required_version)
AC_SUBST([AVFILTER_REQUIRED_VERSION], avfilter_required_version)
AC_SUBST([AVFORMAT_REQUIRED_VERSION], avformat_required_version)
AC_SUBST([AVUTIL_REQUIRED_VERSION], avutil_required_version)
AC_SUBST([CURL_REQUIRED_VERSION], curl_required_version)
AC_SUBST([LIBZIP_REQUIRED_VERSION], libzip_required_version)
AC_SUBST([EXPAT_REQUIRED_VERSION], expat_required_version)
AC_SUBST([NCLUA_REQUIRED_VERSION], nclua_required_version)
AC_SUBST([SDL2_IMAGE_REQUIRE_VERSION], sdl2_image_require_version)

AC_SUBST([SDL2_REQUIRED_VERSION], sdl2_required_version)
AC_SUBST([SDL2_TTF_REQUIRED_VERSION], sdl2_ttf_required_version)
AC_SUBST([SWRESAMPLE_REQUIRED_VERSION], swresample_required_version)
AC_SUBST([SWSCALE_REQUIRED_VERSION], swscale_required_version)
AC_SUBST([XERCESC_REQUIRED_VERSION], xercesc_required_version)
AC_SUBST([LIBRSVG_REQUIRED_VERSION], librsvg_required_version)
AC_SUBST([LIBBPG_REQUIRED_VERSION], libbpg_required_version)

AM_INIT_AUTOMAKE([-Wall 1.11 foreign gnu no-define subdir-objects])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])
AM_PROG_AR

AC_PROG_CXX
AC_LANG_CPLUSPLUS

LT_PREREQ([2.2])
LT_INIT([disable-static win32-dll])

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

# _AC_DEFINE_DEPRECATED(NAME, VALUE)
# ------------------------------
# Define deprecated macro with name NAME and value VALUE.
# Eventually, these should be removed from the code.
m4_define([_AC_DEFINE_DEPRECATED],
 [AC_DEFINE([$1], [$2], [Deprecated macro.])])

# Enable data-processing.
_AC_ARG_ENABLE([data-processing], [build data-processing support], [no])
if test "$enable_data_processing" = yes; then
  AC_DEFINE([HAVE_DATA_PROCESSING], [1],
   [Define to 1 if data-processing is enabled.])
  _AC_DEFINE_DEPRECATED([HAVE_TUNER], [1])
  _AC_DEFINE_DEPRECATED([HAVE_TSPARSER], [1])
  _AC_DEFINE_DEPRECATED([HAVE_DSMCC], [1])
else
  AC_DEFINE([HAVE_DATA_PROCESSING], [0],
   [Define to 1 if data-processing is enabled.])
  _AC_DEFINE_DEPRECATED([HAVE_TUNER], [0])
  _AC_DEFINE_DEPRECATED([HAVE_TSPARSER], [0])
  _AC_DEFINE_DEPRECATED([HAVE_DSMCC], [0])
fi
AM_CONDITIONAL([BUILD_DATA_PROCESSING],
 [test "$enable_data_processing" = yes])

# Enable multi-device.
_AC_ARG_ENABLE([multidevice], [build multi-device support], [no])
if test "$enable_multidevice" = yes; then
  AC_CHECK_PROGS([CONVERT], [convert])
  if test -z "$CONVERT"; then
    AC_MSG_WARN([
***
*** Checks for CONVERT failed.
*** ImageMagick's convert utility is required by Ginga-CC Multi-device.
*** If convert is not installed, some applications may not work properly.
***])
  fi
  AC_DEFINE([HAVE_MULTIDEVICE], [1],
   [Define to 1 if multidevice is enabled.])
else
  AC_DEFINE([HAVE_MULTIDEVICE], [0],
   [Define to 1 if multidevice is enabled.])
fi
AM_CONDITIONAL([BUILD_MULTIDEVICE], [test "$enable_multidevice" = yes])

# Enable tests.
_AC_ARG_ENABLE([tests], [build unit tests], [no])
AM_CONDITIONAL([BUILD_TESTS], [test "$enable_tests" = yes])

# Enable NCLua trace.
_AC_ARG_ENABLE([nclua-trace], [trace NCLua calls], [no])
if test "$enable_nclua_trace" = yes; then
  AC_DEFINE([LUAPLAYER_ENABLE_TRACE], [1],
    [Define to 1 to trace NCLua calls.])
else
  AC_DEFINE([LUAPLAYER_ENABLE_TRACE], [0],
    [Define to 1 to trace NCLua calls.])
fi

# Enable linux dvb support.
_AC_ARG_ENABLE([linuxdvb], [build linuxdvb support], [no])
if test "$enable_linuxdvb" = yes; then
  AC_CHECK_HEADER([linux/dvb/frontend.h],
   [linuxdvb_ok=yes],
   [linuxdvb_ok=no])
  if test "$linuxdvb_ok" = yes; then
    AC_DEFINE([HAVE_LINUXDVB], [1], [Define to 1 if LinuxDVB is present.])
  else
    AC_MSG_ERROR([
*** Checks for LinuxDVB failed.
*** LinuxDVB support is optional; you can disable it by passing
*** the option --disable-linuxdvb to configure.])
  fi
fi
AM_CONDITIONAL([BUILD_LINUXDVB], [test "$enable_linuxdvb"=yes])

# Check if we're on MS Windows.
case "$target_os" in
  *-*-mingw*)
    os_win=yes
    ;;
  *)
    os_win=no
esac
AM_CONDITIONAL([OS_WINDOWS], [test "$os_win" = yes])

# Check for pthreads (required by most packages).
AX_PTHREAD([],
 [AC_MSG_ERROR([
*** Checks for pthreads failed.])])

# Check for Expat (required Ginga-CC Data Processing).
AX_LIB_EXPAT([expat_required_version])
if test "$HAVE_EXPAT" = no; then
  AC_MSG_ERROR([
*** Expat XML Parser >= expat_required_version is required.])
fi

############################# Ginga-CC System ##############################

# Socket and sysinfo.
AC_CHECK_HEADER([sys/socket.h])
AC_CHECK_HEADER([net/if.h])
AC_CHECK_HEADER([sys/sysinfo])

# Check for libzip if multi-device support is enabled.
PKG_CHECK_MODULES([LIBZIP],
  [libzip >= libzip_required_version],
  [have_libzip=yes],
  [have_libzip=no])

if test "$enable_multidevice" = yes; then
  if test "$have_libzip" = yes; then
    AC_DEFINE([HAVE_ZIP], [1], [Define to 1 if libzip is present.])
  else
      AC_MSG_ERROR([
*** libzip >= libzip_required_version is required by --enable-multidevice.])
  fi
fi

############################### Ginga-CC IC ################################

# Check for Curl.
AX_LIB_CURL([curl_required_version], [:],
 [AC_MSG_ERROR([
*** CURL >= curl_required_version is required.])])

# FIXME: This should be removed since it is always defined.
_AC_DEFINE_DEPRECATED([HAVE_IC], [1])

############################### Ginga-CC MB ################################

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

# Check for libjpeg.
if test "$enable_multidevice" = yes; then
  AC_CHECK_LIB([jpeg], [jpeg_destroy_decompress],
   [jpeg_ok=yes],
   [jpeg_ok=no; AC_MSG_WARN([*** JPEG library not found])])

  if test "$jpeg_ok" = yes; then
    AC_MSG_CHECKING([for jpeglib.h])
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

    if test "$jpeg_ok" = yes; then
      JPEG_LIBS='-ljpeg'
      _AC_DEFINE_DEPRECATED([HAVE_JPEG], [1])
    else
      AC_MSG_WARN([*** JPEG header file not found])
    fi
  fi

  if test "$with_libjpeg" != no && test -z "$JPEG_LIBS"; then
    AC_MSG_ERROR([
*** libjpeg is required by --enable-multidevice.])
  fi
fi
AC_SUBST([JPEG_LIBS])

############################# Ginga-CC Player ##############################

# Check for Berkelium.
berkelium_ok=no
_AC_ARG_ENABLE([berkelium], [build with Berkelium XHTML support], [yes])
if test "$enable_berkelium" = yes; then
  AC_HAVE_LIBRARY([berkelium],
   [AC_CHECK_HEADER([berkelium/Berkelium.hpp],
     [berkelium_ok=yes])])
  if test "$berkelium_ok" = yes; then
    AC_DEFINE([HAVE_BERKELIUM], [1],
      [Define to 1 if berkelium is present.])
    BERKELIUM_LIBS='-lberkelium'
  else
    AC_DEFINE([HAVE_BERKELIUM], [0],
      [Define to 0 if berkelium is not present.])
    AC_MSG_WARN([
  ***
  *** Checks for BERKELIUM failed.
  *** Berkelium is required by Ginga-CC Player.
  *** If Berkelium is not installed, some applications may not work properly.
  ***])
  fi
  AC_SUBST([BERKELIUM_LIBS])
fi
AM_CONDITIONAL([BUILD_BERKELIUM], [test "$berkelium_ok" = yes])

# Check for espeak.
espeak_ok=no
_AC_ARG_ENABLE([espeak], [build with espeak SSML support], [no])
if test "$enable_espeak" = yes; then
  AC_HAVE_LIBRARY([espeak],
   [AC_CHECK_HEADER([espeak/speak_lib.h],
     [espeak_ok=yes])])
  if test "$espeak_ok" = yes; then
    AC_DEFINE([HAVE_SSML], [1],
      [Define to 1 if espeak is present.])
    SSML_LIBS='-lespeak'
  else
    AC_DEFINE([HAVE_SSML], [0],
      [Define to 0 if espeak is not present.])
    AC_MSG_WARN([
  ***
  *** Checks for eSpeak failed.
  *** eSpeak is required by Ginga-CC Player.
  *** If eSpeak is not installed, some applications may not work properly.
  ***])
  fi
  AC_SUBST([SSML_LIBS])
fi
AM_CONDITIONAL([BUILD_SSML], [test "$enable_espeak" = yes])

# Check for LibRsvg for SVG
librsvg_ok=no
_AC_ARG_ENABLE([librsvg], [build with LibRsvg SVG support], [no])
if test "$enable_librsvg" = yes; then
  PKG_CHECK_MODULES([LIBRSVG],
   [librsvg-2.0 >= librsvg_required_version], [librsvg_ok=yes],
    [AC_MSG_ERROR([
*** librsvg >= librsvg_required_version is required.])])

  if test "$librsvg_ok" = yes; then
    AC_DEFINE([HAVE_LIBRSVG], [1],
      [Define to 1 if librsvg is present.])
  else
    AC_DEFINE([HAVE_LIBRSVG], [0],
      [Define to 0 if librsvg is not present.])
  fi
fi
AM_CONDITIONAL([BUILD_SVG], [test "$enable_librsvg" = yes])

# Check for libbpg for BPG support
libbpg_ok=no
_AC_ARG_ENABLE([libbpg], [build with libbpg BPG support], [no])
if test "$enable_libbpg" = yes; then
  PKG_CHECK_MODULES([LIBBPG],
   [libbpg >= libbpg_required_version], [libbpg_ok=yes],
    [AC_MSG_ERROR([
*** libbpg >= libbpg_required_version is required.])])

  if test "$libbpg_ok" = yes; then
    AC_DEFINE([HAVE_LIBBPG], [1],
      [Define to 1 if libbpg is present.])
  else
    AC_DEFINE([HAVE_LIBBPG], [0],
      [Define to 0 if libbpg is not present.])
  fi
fi
AM_CONDITIONAL([BUILD_LIBBPG], [test "$enable_libbpg" = yes])

# Check for NCLua.
PKG_CHECK_MODULES([NCLUA],
 [nclua >= nclua_required_version], [],
 [AC_MSG_ERROR([
*** nclua >= nclua_required_version is required.])])

############################ NCL 3.0 Converter #############################

# Check for xerces-c.
AX_XERCESC
AS_IF([test "$HAVE_LIBXERCES_C" = no],
 [AC_MSG_ERROR([
*** xerces-c >= xercesc_required_version is required.])])
AC_SUBST([XERCESC_CFLAGS], [])
AC_SUBST([XERCESC_LIBS], [$LIBXERCES_C])

############################################################################

# Ginga version.
AC_DEFINE_UNQUOTED([VERSION], ["$PACKAGE_VERSION"], [Ginga version.])

# Ginga confdir, libdir, and includedir.
AC_SUBST([gingaconfdir], ['${sysconfdir}'/ginga/files])
AC_SUBST([gingalibdir], ['${libdir}'])
AC_SUBST([gingaincludedir], ['${includedir}'/ginga])

# Per-package libdir and includedir.
m4_define([_AC_SUBST_GINGAPKGDIR],
 [AC_SUBST([$1][libdir], ['${gingalibdir}'])
  AC_SUBST([$1][includedir], ['${gingaincludedir}'/][$2])])
_AC_SUBST_GINGAPKGDIR([gingacccm],             [cm])
_AC_SUBST_GINGAPKGDIR([gingaccmultidevice],    [multidevice])
_AC_SUBST_GINGAPKGDIR([gingacctuner],          [tuner])
_AC_SUBST_GINGAPKGDIR([gingacctsparser],       [tsparser])
_AC_SUBST_GINGAPKGDIR([gingaccdataprocessing], [dataprocessing])
_AC_SUBST_GINGAPKGDIR([ncl30],                 [ncl30])
_AC_SUBST_GINGAPKGDIR([ncl30converter],        [converter])
_AC_SUBST_GINGAPKGDIR([gingancl],              [gingancl])

AC_CONFIG_FILES([
Makefile
data/Makefile
ginga/Makefile
ctxmgmt/Makefile
gingacc-dataprocessing/Makefile
mb/Makefile
gingacc-multidevice/Makefile
player/Makefile
system/Makefile
gingacc-tsparser/Makefile
gingacc-tuner/Makefile
lssm/Makefile
gingancl/Makefile
ic/Makefile
ncl30-converter/Makefile
ncl30/Makefile
util/Makefile
])

AC_CONFIG_FILES([
mb/tests/Makefile
gingacc-multidevice/tests/Makefile
gingacc-tuner/tests/Makefile
gingacc-tsparser/tests/Makefile
gingacc-dataprocessing/tests/Makefile
player/tests/Makefile
gingancl/tests/Makefile
])

AC_OUTPUT
AC_MSG_NOTICE([summary of main build options:

  version:            ${VERSION}
  host type:          ${host}
  install prefix:     ${prefix}
  compiler:           CC: \"${CC}\"  CFLAGS: \"${CFLAGS}\"  CPPFLAGS: \"${CPPFLAGS}\"  LDFLAGS: \"${LDFLAGS}\"
  library types:      shared=${enable_shared}, static=${enable_static}

  build data-processing:  ${enable_data_processing}
  build multi-device:     ${enable_multidevice}
  build tests:            ${enable_tests}
  build linuxdvb:         ${enable_linuxdvb}
  build berkelium:        ${berkelium_ok}
  build espeak:           ${enable_espeak}
  build librsvg:          ${enable_librsvg}
  build libbpg:           ${enable_libbpg}

  install headers:        ${enable_headers}
  trace nclua calls:      ${enable_nclua_trace}
])
