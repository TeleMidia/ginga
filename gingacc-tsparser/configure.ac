AC_INIT(gingacctsparser, 0.13.1)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
#AM_PROG_LIBTOOL

CXXFLAGS="-fPIC $CXXFLAGS"
CFLAGS="-fPIC $CFLAGS"

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

#TODO: Search telemidia-util, gingacc-tuner ... include paths 
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

##########################
# Ginga CC System Thread #
##########################
AC_CHECK_HEADER([system/thread/Thread.h], [], AC_MSG_ERROR([

Couldn't find Ginga CC System Thread headers.
Please install Ginga CC System Thread Headers.

]))

##################
# Ginga CC Tuner #
##################
AC_CHECK_HEADER([tuner/ITuner.h], [], AC_MSG_ERROR([

Couldn't find Ginga CC Tuner headers.
Please install Ginga CC Tuner Headers.

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

####################
# Ginga CC System  #
####################
AC_HAVE_LIBRARY(gingaccsystem, , AC_MSG_ERROR([

Couldn't find Ginga CC System Thread library.
Please install Ginga CC System Thread Library.

]))

##################
# Ginga CC Tuner #
##################
AC_HAVE_LIBRARY(gingacctuner, , AC_MSG_ERROR([

Couldn't find Ginga CC Tuner library.
Please install Ginga CC Tuner Library.

]))

#################
# Debug Support #
#################
debug=no
AC_ARG_ENABLE(debug,
  [  --enable-debug build project with debug support [[default=no]]],
  enable_debug="$enableval", enable_debug=no)

if test x$enable_debug = xno; then
   AC_DEFINE(HAVE_DEBUG, 0, [Def to 0 if you don't want debug support.])
   AC_MSG_WARN([*** Debug support will not be built.])

else
   AC_DEFINE(HAVE_DEBUG, 1, [Def to 1 if you want debug support.])
   AC_MSG_RESULT([*** Debug support will be built.])
fi

############################################
# Offering a non-componentized alternative #
############################################
components=yes
AC_ARG_ENABLE(components,
  [  --enable-components build project using components [[default=yes]]],
  enable_components="$enableval", enable_components=yes)

if test x$enable_components = xno; then
   COMPONENTS_LIBS='-L/usr/local/lib/ -L/usr/local/lib/ginga/'
   AC_DEFINE(HAVE_COMPSUPPORT, 0, [Def to 0 if you don't have Component support.])
   AC_MSG_WARN([*** Component support will not be built.])

else
   COMPONENTS_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/'
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

AM_CONDITIONAL([MULTIPROCESS], [test x$enable_multiprocess = xyes])
AC_SUBST(MULTIPROCESS_LIBS)

AC_CONFIG_FILES(Makefile
	include/tsparser/Makefile
	src/Makefile
	test/Makefile)

AC_OUTPUT

AC_MSG_RESULT([
Build options: 
  Module directory        $MODULEDIR
  Component Support       $enable_components
  Multi Process Support   $enable_multiprocess
])
