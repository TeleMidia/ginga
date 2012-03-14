AC_INIT(gingaccmb, 0.13.1)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
AC_PROG_LIBTOOL

DIRECTFB_MAJOR_VERSION=1
DIRECTFB_MINOR_VERSION=0
DIRECTFB_MICRO_VERSION=0
DIRECTFB_INTERFACE_AGE=0
DIRECTFB_BINARY_AGE=0
DIRECTFB_VERSION=$DIRECTFB_MAJOR_VERSION.$DIRECTFB_MINOR_VERSION.$DIRECTFB_MICRO_VERSION

AC_SUBST(DIRECTFB_MAJOR_VERSION)
AC_SUBST(DIRECTFB_MINOR_VERSION)
AC_SUBST(DIRECTFB_MICRO_VERSION)
AC_SUBST(DIRECTFB_INTERFACE_AGE)
AC_SUBST(DIRECTFB_BINARY_AGE)
AC_SUBST(DIRECTFB_VERSION)

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib /usr/local/lib /usr/local/lib/ginga; do
if test -d ${spfx}; then
LDFLAGS="-L${spfx} $LDFLAGS"
fi
done

for spfx in /usr/include /usr/local/include /usr/local/include/ginga /usr/local/include/directfb /usr/local/include/SDL2; do
if test -d ${spfx}; then
CPPFLAGS="-I${spfx} $CPPFLAGS"
fi
done

###########
# HEADERS #
###########

##################
# TeleMidia Util #
##################

AC_CHECK_HEADER([util/functions.h], [], AC_MSG_ERROR([

Couldn't find TeleMidia Util headers.
Please install TeleMidia Util Headers.

]))

########
# LIBS #
########

##################
# TeleMidia Util #
##################

AC_HAVE_LIBRARY(telemidiautil, , AC_MSG_ERROR([

Couldn't find TeleMidia Util library.
Please install TeleMidia Util Library.

]))

######################
# Finding pkg-config #
######################
haspkgconfig=yes
AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test x$PKG_CONFIG = xno ; then
AC_MSG_WARN([*** pkg-config not found. See http://pkgconfig.sourceforge.net])
haspkgconfig=no
fi

if ! pkg-config --atleast-pkgconfig-version 0.9 ; then
AC_MSG_WARN([*** pkg-config too old; version 0.9 or better required.])
haspkgconfig=no
fi

###################
# DirectFB System #
###################
DFB_LIBS='-L/usr/local/lib/ginga'

if test x$haspkgconfig = xyes; then
dfb=yes
AC_ARG_ENABLE(dfb,
  [  --enable-dfb build project with DirectFB support [[default=yes]]],
  enable_dfb="$enableval", enable_dfb=yes)

if test x$enable_dfb = xno; then
AC_MSG_WARN([
*** Ginga DirectFB support will not be built.
])
else
DIRECTFB_MAX_VERSION=1.2.0
DIRECTFB_MIN_VERSION=$DIRECTFB_VERSION

AC_MSG_CHECKING(for DirectFB == $DIRECTFB_MAN_VERSION)
if $PKG_CONFIG --atleast-version $DIRECTFB_MAX_VERSION directfb ; then
DFB_CFLAGS=`$PKG_CONFIG --cflags directfb`
DFB_LIBS=`$PKG_CONFIG --libs directfb`
DFB_VERSION=`$PKG_CONFIG --modversion directfb`
AC_MSG_RESULT([found (version $DFB_VERSION)])
AC_DEFINE(DFB_VER, 120, [Version Control.])
else
if $PKG_CONFIG --atleast-version $DIRECTFB_MIN_VERSION directfb ; then
DFB_CFLAGS=`$PKG_CONFIG --cflags directfb`
DFB_LIBS=`$PKG_CONFIG --libs directfb`
DFB_VERSION=`$PKG_CONFIG --modversion directfb`
AC_MSG_RESULT([found (version $DFB_VERSION)])
AC_DEFINE(DFB_VER, 100, [Version Control.])
else
AC_MSG_WARN([
*** Ginga DirectFB support will not be built.
*** DirectFB $DIRECTFB_REQUIRED_VERSION or newer is required.
Try also "export PKG_CONFIG_PATH=..." and run configure again
])
enable_dfb=no
fi
fi
fi #enable_dfb
else #haspkgconfig
AC_MSG_WARN([
*** Ginga DirectFB support will not be built.
*** DirectFB $DIRECTFB_REQUIRED_VERSION or newer is required.
Try also "export PKG_CONFIG_PATH=..." and run configure again
])
enable_dfb=no
fi #haspkgconfig

if test x$enable_dfb = xno; then
AC_DEFINE(HAVE_DIRECTFB, 0, [Define to 0 if you don't have DFB support.])
else
AC_DEFINE(HAVE_DIRECTFB, 1, [Define to 1 if you have DFB support.])
fi

AM_CONDITIONAL([DFBSYSTEM], [test x$enable_dfb = xyes])
AC_SUBST(DFB_LIBS)

###################
# Terminal System #
###################
term=no
AC_ARG_ENABLE(term,
  [  --enable-term build project with Terminal System support [[default=no]]],
  enable_term="$enableval", enable_term=no)

if test x$enable_term = xno; then
AC_MSG_WARN([
*** Ginga Terminal support will not be built.
])

AC_DEFINE(HAVE_TERM, 0, [Define to 0 if you don't have Term support.])
else

AC_DEFINE(HAVE_TERM, 1, [Define to 1 if you have Term support.])
fi

AM_CONDITIONAL([TERMSYSTEM], [test x$enable_term = xyes])

##############
# SDL System #
##############
sdl=no
AC_ARG_ENABLE(sdl,
  [  --enable-sdl build project with SDL System support [[default=no]]],
  enable_sdl="$enableval", 
  enable_sdl=no
)

if test x$enable_sdl = xyes; then
AC_HAVE_LIBRARY(SDL2, [], enable_sdl=no)
fi

if test x$enable_sdl = xno; then
AC_MSG_WARN([*** SDL2 lib not Found!])
AC_MSG_WARN([*** Ginga SDL2 support will not be built.])
fi

AM_CONDITIONAL([SDLSYSTEM], [test x$enable_sdl = xyes])

#####################
# ffmpeg for SDL MB #
#####################
sdlffmpeg=yes

if test x$enable_sdl = xno; then
sdlffmpeg=no
else
AC_HAVE_LIBRARY(avcodec, [], sdlffmpeg=no)
fi

if test x$sdlffmpeg = xno; then
AC_MSG_WARN([*** ffmpeg libraries not Found, disabling SDL support.])
enable_sdl=no
fi

AM_CONDITIONAL([SDLSYSTEM], [test x$enable_sdl = xyes])

#############
# SDL_image #
#############
sdlimage=yes

if test x$enable_sdl = xno; then
sdlimage=no
else
AC_HAVE_LIBRARY(SDL2_image, [], sdlimage=no)
fi

if test x$sdlimage = xno; then
AC_MSG_WARN([*** SDL2_image lib not Found, disabling SDL support.])
enable_sdl=no
sdlffmpeg=no
fi

AM_CONDITIONAL([SDLSYSTEM], [test x$enable_sdl = xyes])

###########
# SDL_ttf #
###########
sdlttf=yes

if test x$enable_sdl = xno; then
sdlttf=no
else
AC_HAVE_LIBRARY(SDL2_ttf, [], sdlttf=no)
fi

if test x$sdlttf = xno; then
AC_MSG_WARN([*** SDL2_ttf lib not Found, disabling SDL support.])
sdlffmpeg=no
sdlimage=no
enable_sdl=no
fi

if test x$enable_sdl = xno; then
AC_DEFINE(HAVE_SDL, 0, [Define to 0 if you don't have SDL support.])
else
AC_DEFINE(HAVE_SDL, 1, [Define to 1 if you have SDL support.])
fi

AM_CONDITIONAL([SDLSYSTEM], [test x$enable_sdl = xyes])

############################################################
# Checking if we have at least 1 multimedia backend system #
############################################################

if test x$enable_dfb = xno; then
if test x$enable_term = xno; then
if test x$enable_sdl = xno; then
AC_MSG_ERROR([
*** No multimedia backend system found. Ginga cannot be built.
])
fi
fi
fi


################
# Fusion Sound #
################
fusionsound=yes
AC_ARG_ENABLE(fusionsound,
  [  --enable-fusionsound build project with FusionSound support [[default=yes]]],
  enable_fusionsound="$enableval", enable_fusionsound=yes)

if test x$enable_dfb = xno; then
enable_fusionsound=no
else
AC_HAVE_LIBRARY(
  fusionsound,
  [],
  AC_MSG_WARN([*** fusion sound lib not Found.])
    enable_fusionsound=no
)

if test x$enable_fusionsound = xno; then
   FUSIONSOUND_LIBS='-L/usr/local/lib/ginga'
   AC_DEFINE(HAVE_FUSIONSOUND, 0, [Define to 0 if you don't have FusionSound support.])
   AC_MSG_WARN([*** Ginga FusionSound support will not be built.])
else
   FUSIONSOUND_LIBS=`$PKG_CONFIG --libs fusionsound`
   AC_DEFINE(HAVE_FUSIONSOUND, 1, [Define to 1 if you have FusionSound support.])
   AC_MSG_RESULT([*** Ginga FusionSound support will be built.])
fi
fi

AM_CONDITIONAL([FUSIONSOUND], [test x$enable_fusionsound = xyes])
AC_SUBST(FUSIONSOUND_LIBS)

DVC_LIBS=

#############################################
# Offering a non-componentized alternative #
############################################
components=yes
force_xineprovider=no
AC_ARG_ENABLE(components,
  [  --enable-components build project using components [[default=yes]]],
  enable_components="$enableval", enable_components=yes)

AC_CHECK_HEADER(
  [cm/IComponentManager.h],
  [],
  AC_MSG_WARN([*** Component Manager not Found.])
    enable_components=no
)

if test x$enable_components = xno; then
   TEST_LIBS='-lgingaccmbdfb -lgingaccmbdecdfbimg -lgingaccmbdecdfbav -L/usr/local/lib/ginga/mb -L/usr/local/lib/ginga/mb/dec'
   COMPONENTS_LIBS='-L/usr/local/lib/ -L/usr/local/lib/ginga/ -L/usr/local/lib/ginga/mb -L/usr/local/lib/ginga/mb/dec'
   AC_DEFINE(HAVE_COMPSUPPORT, 0, [Def to 0 if you don't have Component support.])
   AC_MSG_WARN([*** Component support will not be built.])
   force_xineprovider=yes
   AC_MSG_WARN([*** Xine provider will be forced in a non-componentized built.])
else
   TEST_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/ -L/usr/local/lib/ginga/mb -L/usr/local/lib/ginga/mb/dec'
   COMPONENTS_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   AC_DEFINE(HAVE_COMPSUPPORT, 1, [Def to 1 if you have Component support.])
   AC_MSG_RESULT([*** Component support will be built.])
fi

AM_CONDITIONAL([COMPONENTS], [test x$enable_components = xyes])
AC_SUBST(TEST_LIBS)
AC_SUBST(COMPONENTS_LIBS)

########################
# XINE MAIN AV Support #
########################
xineprovider=no
AC_ARG_ENABLE(xineprovider,
  [  --enable-xineprovider build project with xine provider support (in order to decode main A/V) [[default=no]]],
  enable_xineprovider="$enableval", enable_xineprovider=no)

if test x$enable_dfb = xno; then
enable_xineprovider=no
else

AC_CHECK_HEADER(
  [xine.h],
  [],
  AC_MSG_WARN([*** xine header not Found.])
  enable_xineprovider=no
  if test x$force_xineprovider = yes; then
    AC_MSG_ERROR(["Xine not found!"])
  fi
)

if test x$force_xineprovider = xyes; then
  enable_xineprovider=yes
fi

if test x$enable_xineprovider = xno; then
   AC_MSG_WARN([*** Ginga Xine provider support will not be built.])
   AC_DEFINE(HAVE_XINEPROVIDER, 0, [Def to 0 if you don't have ginga xine provider support.])

else
   AC_MSG_RESULT([*** Ginga Xine provider support will be built.])
   AC_DEFINE(HAVE_XINEPROVIDER, 1, [Def to 1 if you don't have ginga xine provider support.])
   DVC_LIBS='/usr/src/ginga/deps/DirectFB-extra/lib/dvc/libdvc.la'
fi
fi

AM_CONDITIONAL([XINEPROVIDER], [test x$enable_xineprovider = xyes])

##########################
# FFMPEG MAIN AV Support #
##########################
ffmpegprovider=no
AC_ARG_ENABLE(ffmpegprovider,
  [  --enable-ffmpegprovider build project with xine provider support (in order to decode main A/V) [[default=no]]],
  enable_ffmpegprovider="$enableval", enable_ffmpegprovider=no)

if test x$enable_dfb = xno; then
enable_ffmpegprovider=no
else
AC_CHECK_HEADER(
  [dvc/dvc.h],
  [],
  AC_MSG_WARN([*** dvc header not Found.])
    enable_ffmpegprovider=no
)

if test "x$enable_ffmpegprovider" = "xyes"; then
    AC_MSG_CHECKING([for libavcodec/libavformat])
    if $PKG_CONFIG libavcodec libavformat; then
        FFMPEG_CFLAGS=`$PKG_CONFIG --cflags libavcodec libavformat`
        FFMPEG_LIBS=`$PKG_CONFIG --libs libavcodec libavformat`
    else
        FFMPEG_CFLAGS='-L/usr/local/lib'
        FFMPEG_LIBS='-L/usr/local/lib'
        enable_ffmpegprovider=no
    fi
fi

if test x$enable_ffmpegprovider = xno; then
   AC_MSG_WARN([*** Ginga FFmpeg provider support will not be built.])
   AC_DEFINE(HAVE_FFMPEGPROVIDER, 0, [Def to 0 if you don't have ginga ffmpeg provider support.])

else
   AC_MSG_RESULT([*** Ginga FFmpeg provider support will be built.])
   AC_DEFINE(HAVE_FFMPEGPROVIDER, 1, [Def to 1 if you have ginga ffmpeg provider support.])
   DVC_LIBS='/usr/src/ginga/deps/DirectFB-extra/lib/dvc/libdvc.la'
fi
fi

AC_SUBST(FFMPEG_CFLAGS)
AC_SUBST(FFMPEG_LIBS)
AC_SUBST(DVC_LIBS)
AM_CONDITIONAL([FFMPEGPROVIDER], [test x$enable_ffmpegprovider = xyes])

##################
# Kinect Support #
##################
kinect=no
AC_ARG_ENABLE(kinect,
  [  --enable-kinect build project with kinect support (kinect as an input device) [[default=no]]],
  enable_kinect="$enableval", enable_kinect=no)
   
KINECT_LIBS='-L/usr/local/lib/ginga/'
KINECT_INCS='-L/usr/local/include/ginga/'

if test x$enable_kinect = xno; then
   AC_DEFINE(HAVE_KINECTSUPPORT, 0, [Def to 0 if you don't have Kinect support.])
   AC_MSG_WARN([*** Kinect support will not be built.])
else

AC_HAVE_LIBRARY(
    XnVNite_1_4_1,
    enable_kinect=yes,
    AC_MSG_WARN([Couldn't find XnVNite_1_4_1.])
    enable_kinect=no
)

if test x$enable_kinect = xno; then
   AC_DEFINE(HAVE_KINECTSUPPORT, 0, [Def to 0 if you don't have Kinect support.])
   AC_MSG_WARN([*** Kinect support will not be built.])
else
   KINECT_LIBS='-lOpenNI -lXnVNite_1_4_1'
   KINECT_INCS='-I/usr/include/nite -I/usr/include/ni -I/usr/include/sensor -I/usr/include/sensor/XnEE'
   AC_DEFINE(HAVE_KINECTSUPPORT, 1, [Def to 1 if you have Kinect support.])
   AC_MSG_RESULT([*** Kinect support will be built.])
fi
fi

AM_CONDITIONAL([KINECT], [test x$enable_kinect = xyes])
AC_SUBST(KINECT_LIBS)
AC_SUBST(KINECT_INCS)

######################
# Windowless Support #
######################
windowless=no
AC_ARG_ENABLE(windowless,
  [  --enable-windowless build project with windowless support [[default=no]]],
  enable_windowless="$enableval", enable_windowless=no)

if test x$enable_windowless = xyes; then
   AC_MSG_RESULT([*** Ginga Windowless support will be built.])
   AC_DEFINE(HAVE_WINDOWLESS, 1, [Def to 1 if you have ginga windowless support.])
else
   AC_MSG_RESULT([*** Ginga Windowless support will not be built.])
   AC_DEFINE(HAVE_WINDOWLESS, 0, [Def to 0 if you don't have ginga windowless support.])
fi

AC_CONFIG_FILES(
	Makefile
	include/mb/Makefile
	include/mb/interface/Makefile
	src/Makefile
	src/interface/dfb/Makefile
	src/interface/dfb/content/audio/Makefile
	src/interface/dfb/content/image/Makefile
	src/interface/dfb/content/text/Makefile
	src/interface/dfb/content/video/Makefile
	src/interface/sdl/Makefile
	src/interface/sdl/content/audio/Makefile
	src/interface/sdl/content/image/Makefile
	src/interface/sdl/content/text/Makefile
	src/interface/sdl/content/video/Makefile
	src/interface/term/Makefile
	src/interface/term/content/audio/Makefile
	src/interface/term/content/image/Makefile
	src/interface/term/content/text/Makefile
	src/interface/term/content/video/Makefile
	files/Makefile
	test/Makefile
)

AC_OUTPUT

AC_MSG_RESULT([
Build options:
  Module directory                 $MODULEDIR
   
  Mulimedia Backend Systems:
  DirectFB System Support          $enable_dfb
  Terminal System Support          $enable_term
  SDL System Support               $enable_sdl
  
  DFB Providers:
  FusionSound                      $enable_fusionsound
  Xine Main AV                     $enable_xineprovider
  FFmpeg Main AV                   $enable_ffmpegprovider
  
  SDL Providers:
  SDL2_image                       $sdlimage
  SDL2_ttf                         $sdlttf
  ffmpeg                           $sdlffmpeg

  Component Support                $enable_components
  
  Kinect (input device) Support    $enable_kinect
])
