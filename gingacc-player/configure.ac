AC_INIT(gingaccplayer, 0.13.1)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
#AM_PROG_LIBTOOL

CXXFLAGS="-fPIC $CXXFLAGS"
CFLAGS="-fPIC $CFLAGS"

######################
# Finding pkg-config #
######################
AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
if test x$PKG_CONFIG = xno ; then
AC_MSG_ERROR([*** pkg-config not found. See http://pkgconfig.sourceforge.net])
fi

if ! pkg-config --atleast-pkgconfig-version 0.9 ; then
AC_MSG_ERROR([*** pkg-config too old; version 0.9 or better required.])
fi

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib/ /usr/local/lib/ /usr/local/lib/ginga/ /usr/local/lib/ginga/mbdec /usr/local/lib/ginga/ic; do
if test -d ${spfx}; then
LDFLAGS="-L${spfx} $LDFLAGS"
fi
done

for spfx in /usr/include/ /usr/local/include/ /usr/local/include/ginga/; do
if test -d ${spfx}; then
CPPFLAGS="-I${spfx} $CPPFLAGS"
fi
done
#TODO: Search telemidia-util, gingacc-sytem, ... include paths 
CPPFLAGS='-I${top_srcdir}'"/../../telemidia-util/include $CPPFLAGS"
CPPFLAGS='-I${top_srcdir}'"/../gingacc-system/include $CPPFLAGS"

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

##########################
# Ginga CC System Thread #
##########################
AC_CHECK_HEADER([system/thread/Thread.h], [], AC_MSG_ERROR([

Couldn't find Ginga CC System Thread headers.
Please install gingacc-system project.

]))

#######
# Lua #
#######
AC_CHECK_HEADER([lua.h], [], AC_MSG_ERROR([

Couldn't find Lua headers.
Please install Lua 5.1.2 or later.

]))

###############################
# Ginga CC Multimedia Backend #
###############################
AC_CHECK_HEADER([mb/interface/IWindow.h], [], AC_MSG_ERROR([

Couldn't find Ginga Common Core System I/O headers.
Please install gingacc-system project.

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

##########################
# Ginga CC System Thread #
##########################
AC_HAVE_LIBRARY(gingaccsystem, , AC_MSG_ERROR([

Couldn't find Ginga CC System Thread library.
Please install gingacc-system Project.

]))

########################
# XineProvider Support #
########################

hasxinep=no
AC_HAVE_LIBRARY(gingaccmbdecdfbxineav, hasxinep=yes, hasxinep=no)

if test x$hasxinep = xno; then
   AC_MSG_WARN([*** Xine Provider support will not be built.])
   AC_DEFINE(HAVE_XINEPROVIDER, 0, [Def to 0 if you don't have xine provider support.])

else
   AC_MSG_RESULT([*** Xine Provider support will be built.])
   AC_DEFINE(HAVE_XINEPROVIDER, 1, [Def to 1 if you have xine provider support.])
fi

AM_CONDITIONAL(XINEPROVIDER, test x$hasxinep = xyes)

##########################
# FFmpegProvider Support #
##########################

hasffmpegp=no
AC_HAVE_LIBRARY(gingaccmbdecdfbffmpegav, hasffmpegp=yes, hasffmpegp=no)

if test x$hasffmpegp = xno; then
   AC_MSG_WARN([*** FFmpeg Provider support will not be built.])
   AC_DEFINE(HAVE_FFMPEGPROVIDER, 0, [Def to 0 if you don't have FFmpeg provider support.])

else
   AC_MSG_RESULT([*** FFmpeg Provider support will be built.])
   AC_DEFINE(HAVE_FFMPEGPROVIDER, 1, [Def to 1 if you have FFmpeg provider support.])
fi

AM_CONDITIONAL(FFMPEGPROVIDER, test x$hasffmpegp = xyes)

################################
# GINGA CC Interactive Channel #
################################
hasic=no
AC_HAVE_LIBRARY(gingaccic, hasic=yes, hasic=no)

if test x$hasic = xno; then
   AC_MSG_WARN([*** Ginga-CC Interactive Channel support will not be built.])
   AC_DEFINE(HAVE_IC, 0, [Def to 0 if you don't have gingaccic support.])

else
   AC_MSG_RESULT([*** Ginga-CC Interactive Channel support will be built.])
   AC_DEFINE(HAVE_IC, 1, [Def to 1 if you have gingaccic support.])
fi

AM_CONDITIONAL(IC, test x$hasic = xyes)

############################
# GINGA CC Data Processing #
############################
hasdataprocessing=no
AC_HAVE_LIBRARY(gingaccdataprocessing, hasdataprocessing=yes, hasdataprocessing=no)

if test x$hasdataprocessing = xno; then
   AC_MSG_WARN([*** Ginga-CC Data Processing support will not be built.])
   AC_DEFINE(HAVE_DATAPROC, 0, [Def to 0 if you don't have gingaccdataprocessing support.])
   DATAPROC_LIBS='-lgingaccplayer'

else
   AC_MSG_RESULT([*** Ginga-CC TS Parser support will be built.])
   AC_DEFINE(HAVE_DATAPROC, 1, [Def to 1 if you have gingaccdataprocessing support.])
   DATAPROC_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/ -lgingacctsparser -lgingaccdataprocessing'
fi

AC_SUBST(DATAPROC_LIBS)

##############
# JAVA check #
##############
ncletplayer=no
AC_ARG_ENABLE(ncletplayer,
  [  --enable-ncletplayer    build nclet player support [[default=no]]],
  enable_ncletplayer="$enableval", enable_ncletplayer=no)

if test x$enable_ncletplayer = xyes; then
  AC_HAVE_LIBRARY(jni, [
      AC_CHECK_HEADER(jni.h,
        ncletplayer=yes
        NCLET_LIBS='-ljni'
        NCLET_HEADERS='NCLetPlayer.h'
        AC_DEFINE(HAVE_NCLET, 1, [Def to 1 if you have nclet player support.]),
        AC_MSG_WARN([
*** Headers not found -- nclet player support will not be built.]))
    ], [
      AC_MSG_WARN([
*** jni not found -- nclet player support will not be built.])
    ], -lm)
fi

AM_CONDITIONAL(NCLET, test x$ncletplayer = xyes)
AC_SUBST(NCLET_LIBS)
AC_SUBST(NCLET_HEADERS)

#####################################
# Offering a time stamp alternative #
#####################################
clocktime=yes
AC_ARG_ENABLE(clocktime,
  [  --enable-clocktime build project using clock to player time [[default=yes]]],
  enable_clocktime="$enableval", enable_clocktime=yes)

if test x$enable_clocktime = xno; then
   AC_DEFINE(HAVE_CLOCKTIME, 0, [Def to 0 if you don't have timer support.])
   AC_MSG_WARN([*** Clock time support will not be built (using PTS instead.])

else
   AC_DEFINE(HAVE_CLOCKTIME, 1, [Def to 1 if you have timer support.])
   AC_MSG_RESULT([*** Clock time support will be built.])
fi

AM_CONDITIONAL(clocktime, test "$enable_clocktime" = "no")

############################################
# Offering a non-componentized alternative #
############################################
components=yes
AC_ARG_ENABLE(components,
  [  --enable-components build project using components [[default=yes]]],
  enable_components="$enableval", enable_components=yes)

if test x$enable_components = xno; then
   COMPONENTS_LIBS='-lgingaccmbdecdfbf -lgingaccmbdecdfbfs \
    -lgingaccmbdecdfbimg -lgingaccmbdecdfbav \
    -lgingaccmbdecdfbxineav \
    -lgingaccmb \
    -lgingaccmbdfb -lgingaccsystem -lgingaccic \
    -L/usr/local/lib/ginga/ -L/usr/local/lib \
    -L/usr/local/lib/ginga/mbdec/ \
    -L/usr/local/lib/lua/5.1 -L/usr/local/lib/lua/5.1/socket'
   COMPONENTS_INCS='-I/usr/local/include/ginga -I/usr/local/include/directfb/'
   AC_DEFINE(HAVE_COMPSUPPORT, 0, [Def to 0 if you don't have Component support.])
   AC_MSG_WARN([*** Component support will not be built.])

else
   COMPONENTS_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   COMPONENTS_INCS='-I/usr/local/include/ginga'
   AC_DEFINE(HAVE_COMPSUPPORT, 1, [Def to 1 if you have Component support.])
   AC_MSG_RESULT([*** Component support will be built.])
fi

AM_CONDITIONAL(COMPONENTS, test "$enable_components" = "no")
AC_SUBST(COMPONENTS_LIBS)
AC_SUBST(COMPONENTS_INCS)

#####################
# Berkelium Support #
#####################
berkelium=yes
AC_ARG_ENABLE(berkelium,
  [  --enable-berkelium build project with berkelium player [[default=yes]]],
  enable_berkelium="$enableval", enable_berkelium=yes)

AC_CHECK_HEADER(
  [berkelium/Berkelium.hpp],
  [],
  AC_MSG_WARN([*** Berkelium header not Found.])
    enable_berkelium=no
)

if test x$enable_berkelium = xno; then
   AC_MSG_WARN([*** Ginga Berkelium Player support will not be built.])
   AC_DEFINE(HAVE_BERKELIUM, 0, [Def to 0 if you don't have ginga berkelium support.])

else
   BKL_CFLAGS=`$PKG_CONFIG --cflags glib-2.0`
   BKL_LIBS=`$PKG_CONFIG --libs glib-2.0`
   TEST_LIBS='-lgingaccplabkl -llibberkelium'
   AC_MSG_RESULT([*** Ginga Berkelium Player support will be built.])
   AC_DEFINE(HAVE_BERKELIUM, 1, [Def to 1 if you have ginga berkelium support.])
fi

AM_CONDITIONAL([BERKELIUMLIB], [test x$enable_berkelium = xyes])
AC_SUBST(BKL_LIBS)
AC_SUBST(BKL_CFLAGS)

#################
# Links Support #
#################
links=yes
AC_ARG_ENABLE(links,
  [  --enable-links build project with links player [[default=yes]]],
  enable_links="$enableval", enable_links=yes)

AC_HAVE_LIBRARY(telemidialinks, ,
  AC_MSG_WARN([*** TeleMidia Links Library not Found.])
    enable_links=no
)

if test x$enable_links = xno; then
   AC_MSG_WARN([*** Ginga Links Player support will not be built.])
   AC_DEFINE(HAVE_LINKS, 0, [Def to 0 if you don't have ginga links support.])

else
if test x$enable_berkelium = xno; then
   TEST_LIBS='-lgingaccplalks -ltelemidialinks'
else
   TEST_LIBS='-lgingaccplabkl -llibberkelium -lgingaccplalks -ltelemidialinks'
fi
   AC_MSG_RESULT([*** Ginga Links Player support will be built.])
   AC_DEFINE(HAVE_LINKS, 1, [Def to 1 if you have ginga links support.])
fi

AM_CONDITIONAL([LINKSLIB], [test x$enable_links = xyes])

#####################
# Awesomium Support #
#####################
awesomium=yes
AC_ARG_ENABLE(awesomium,
  [  --enable-awesomium build project with awesomium player [[default=yes]]],
  enable_awesomium="$enableval", enable_awesomium=yes)

AC_CHECK_HEADER(
  [Awesomium/WebCore.h],
  [],
  AC_MSG_WARN([*** Awesomium header not Found.])
    enable_awesomium=no
)

if test x$enable_awesomium = xno; then
   AC_MSG_WARN([*** Ginga Awesomium Player support will not be built.])
   AC_DEFINE(HAVE_AWESOMIUM, 0, [Def to 0 if you don't have ginga awesomium support.])
else # enabling awesomium
if test x$enable_berkelium = xno; then # awesomium without berkelium
if test x$enable_links = xno; then # awesomium without berkelium/links
   TEST_LIBS='-lgingaccplaawe -lawesomium-1.6.3 -L/usr/lib/awesomium-1.6.3'
else # awesomium with links
   TEST_LIBS='-lgingaccplalks -ltelemidialinks -lgingaccplaawe -lawesomium-1.6.3 -L/usr/lib/awesomium-1.6.3'
fi
else # awesomium with berkelium
if test x$enable_links = xno; then # awesomium with berkelium without links
   TEST_LIBS='-lgingaccplabkl -llibberkelium -lgingaccplaawe -lawesomium-1.6.3 -L/usr/lib/awesomium-1.6.3'
else # awesomium with links/berkelium
   TEST_LIBS='-lgingaccplalks -ltelemidialinks -lgingaccplabkl -llibberkelium -lgingaccplaawe -lawesomium-1.6.3 -L/usr/lib/awesomium-1.6.3'
fi
fi
   AC_MSG_RESULT([*** Ginga Awesomium Player support will be built.])
   AC_DEFINE(HAVE_AWESOMIUM, 1, [Def to 1 if you have ginga awesomium support.])
fi

AM_CONDITIONAL([AWESOMIUMLIB], [test x$enable_awesomium = xyes])
AC_SUBST(TEST_LIBS)

###########################
# Multple Process Support #
###########################
multiprocess=no
AC_ARG_ENABLE(multiprocess,
  [  --enable-multiprocess build project with multiple process support [[default=no]]],
  enable_multiprocess="$enableval", enable_multiprocess=no)

if test x$enable_multiprocess = xno; then
   MULTIPROCESS_LIBS='-L/usr/local/lib/ginga/'
   AC_DEFINE(HAVE_MULTIPROCESS, 0, [Def to 0 if you don't have Multiple Process support.])
   AC_MSG_WARN([*** Multiple process support will not be built.])

else
   MULTIPROCESS_LIBS='-lrt -lgingaccsystem -lpthread -L/usr/local/lib/ginga/'
   AC_DEFINE(HAVE_MULTIPROCESS, 1, [Def to 1 if you have Multiple Process support.])
   AC_MSG_RESULT([*** Multiple Process support will be built.])
fi

AM_CONDITIONAL([MULTIPROCESS], [test x$enable_multiprocess = xyes])
AC_SUBST(MULTIPROCESS_LIBS)

AC_CONFIG_FILES(Makefile
	files/Makefile
	files/scripts/Makefile
	include/player/Makefile
	src/Makefile
	src/image/Makefile
	src/application/imperative/lua/Makefile
	src/application/declarative/xhtml/links/Makefile
	src/application/declarative/xhtml/berkelium/Makefile
	src/application/declarative/xhtml/awesomium/Makefile
	src/text/Makefile src/text/plain/Makefile
	src/text/srt/Makefile
	src/video/Makefile
	src/tv/Makefile
	src/widget/Makefile
	tools/Makefile test/Makefile)

AC_OUTPUT

AC_MSG_RESULT([
Build options: 
  Module directory             $MODULEDIR
  Xine Provider Support        $hasxinep
  FFmpeg Provider Support      $hasffmpegp
  Links Player Support         $enable_links
  Berkelium Player Support     $enable_berkelium
  Awesomium Player Support     $enable_awesomium
  NCLET Support                $ncletplayer
  Interactive Channel Support  $hasic
  TS Parser Support            $hasdataprocessing
  Component Support            $enable_components
  Multi Process Support        $enable_multiprocess
])
