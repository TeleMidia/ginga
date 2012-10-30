AC_INIT(gingacc-multidevice, 0.13.4)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib/ /usr/local/lib/ /usr/local/lib/ginga/ ; do
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
CPPFLAGS='-I${top_srcdir}'"/../gingacc-dataprocessing/include $CPPFLAGS"
CPPFLAGS='-I${top_srcdir}'"/../gingacc-contextmanager/include $CPPFLAGS"


###########
# HEADERS #
###########

############################
# Ginga CC Context Manager #
############################
AC_CHECK_HEADER([contextmanager/IContextManager.h], [], AC_MSG_ERROR([

Couldn't find Ginga CC Context Manager headers.
Please install Ginga CC Context Manager Hearders.

]))


##########################
# Ginga-CC System Thread #
##########################
AC_CHECK_HEADER([system/thread/Thread.h], [], AC_MSG_ERROR([

Couldn't find Ginga-CC System Thread headers.
Please install gingacc-system project.

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
   COMPONENTS_LIBS='-L/usr/local/lib/ginga/dp/ -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   TEST_COMPONENTS_LIBS='-lgingaccmd \
      -lgingaccsystem -lgingacccontextmanager \
      -L/usr/local/lib/ginga/dp/ -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   AC_DEFINE(HAVE_COMPSUPPORT, 0, [Def to 0 if you don't have Component support.])
   AC_MSG_WARN([*** Component support will not be built.])

else
   COMPONENTS_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   TEST_COMPONENTS_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   AC_DEFINE(HAVE_COMPSUPPORT, 1, [Def to 1 if you have Component support.])
   AC_MSG_RESULT([*** Component support will be built.])
fi

AM_CONDITIONAL([COMPONENTS], [test x$enable_components = xyes])
AC_SUBST(COMPONENTS_LIBS)
AC_SUBST(TEST_COMPONENTS_LIBS)

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

########
# LIBS #
########

###################
# TeleMidia Util  #
###################
AC_HAVE_LIBRARY(telemidiautil, , AC_MSG_ERROR([

Couldn't find util library.
Please install TeleMidia Util.

]))

###########################
# Ginga CC System Thread  #
###########################
AC_HAVE_LIBRARY(gingaccsystem, , AC_MSG_ERROR([

Couldn't find Ginga-CC System Thread library.
Please install gingacc-system project.

]))

############
# PROGRAMS #
############

###########
# convert #
###########

AC_CHECK_PROG(CONVERT, convert, convert)

if test "x$CONVERT" = "x"; then
	AC_MSG_WARN(Couldn't find convert. Please install ImageMagick.)
fi

AC_CONFIG_FILES(
    Makefile
    include/multidevice/Makefile
    include/multidevice/services/Makefile
    src/Makefile
    test/Makefile)

AC_OUTPUT

AC_MSG_RESULT([
Build options: 
  Module directory        $MODULEDIR
  Component Support       $enable_components
  Multi Process Support   $enable_multiprocess
])
