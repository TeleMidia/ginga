AC_INIT(gingaccdataprocessing, 0.13.4)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib /usr/local/lib /usr/local/lib/ginga; do
if test -d ${spfx}; then
LDFLAGS="-L${spfx} $LDFLAGS"
fi
done

for spfx in /usr/include /usr/local/include /usr/local/include/ginga; do
if test -d ${spfx}; then
CPPFLAGS="-I${spfx} $CPPFLAGS"
fi
done

#TODO: Search telemidia-util, gingacc-system ... include paths 
CPPFLAGS='-I${top_srcdir}'"/../gingacc-system/include $CPPFLAGS"
CPPFLAGS='-I${top_srcdir}'"/../gingacc-tsparser/include $CPPFLAGS"
CPPFLAGS='-I${top_srcdir}'"/../gingacc-tuner/include $CPPFLAGS"
CPPFLAGS='-I${top_srcdir}'"/../../telemidia-util/include $CPPFLAGS"

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

###################
# Ginga CC System #
###################
AC_CHECK_HEADER([system/thread/Thread.h], [], AC_MSG_ERROR([

Couldn't find Ginga CC System Thread headers.
Please install Ginga CC System Thread Headers.

]))

######################
# Ginga CC TS Parser #
######################
AC_CHECK_HEADER([tsparser/ITSPacket.h], [], AC_MSG_ERROR([

Couldn't find Ginga CC TS Parser headers.
Please install Ginga CC TS Parser Headers.

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

###################
# Ginga CC System #
###################
AC_HAVE_LIBRARY(gingaccsystem, , AC_MSG_ERROR([

Couldn't find Ginga CC System Thread library.
Please install Ginga CC System Thread Library.

]))

############################################
# Offering a non-componentized alternative #
############################################
components=yes
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
   COMPONENTS_LIBS='-lgingacctsparser'
   AC_DEFINE(HAVE_COMPSUPPORT, 0, [Def to 0 if you don't have Component support.])
   AC_MSG_WARN([*** Component support will not be built.])

else
   COMPONENTS_LIBS='-ldl'
   AC_DEFINE(HAVE_COMPSUPPORT, 1, [Def to 1 if you have Component support.])
   AC_MSG_RESULT([*** Component support will be built.])
fi

AM_CONDITIONAL([COMPONENTS], [test x$enable_components = xyes])
AC_SUBST(COMPONENTS_LIBS)

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

AM_CONDITIONAL(MULTIPROCESS, test "$enable_multiprocess" = "no")
AC_SUBST(MULTIPROCESS_LIBS)

AC_CONFIG_FILES(Makefile
    include/dataprocessing/ncl/Makefile
    include/dataprocessing/ncl/edit/Makefile
    include/dataprocessing/dsmcc/Makefile
    include/dataprocessing/dsmcc/npt/Makefile
    include/dataprocessing/dsmcc/carousel/object/Makefile
    include/dataprocessing/Makefile
    src/ncl/Makefile
    src/ncl/edit/Makefile
    src/Makefile
    test/Makefile)

AC_OUTPUT

AC_MSG_RESULT([
Build options: 
  Module directory        $MODULEDIR
  Component Support       $enable_components
  Multi Process Support   $enable_multiprocess
])
