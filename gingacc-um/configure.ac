AC_INIT(gingacc-um, 0.13.1)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib/ /usr/local/lib/ /usr/local/lib/ginga; do
if test -d ${spfx}; then
LDFLAGS="-L${spfx} $LDFLAGS"
fi
done

for spfx in /usr/include/ /usr/local/include/ /usr/local/include/ginga/; do
if test -d ${spfx}; then
CPPFLAGS="-I${spfx} $CPPFLAGS"
fi
done

#TODO: Search telemidia-util include path 
CPPFLAGS='-I${top_srcdir}'"/../../telemidia-util/include $CPPFLAGS"
CPPFLAGS='-I${top_srcdir}'"/../gingacc-ic/include $CPPFLAGS"

###########
# HEADERS #
###########

###############
# Ginga-CC CM #
###############
AC_CHECK_HEADER([cm/IComponentManager.h], [], AC_MSG_ERROR([

Couldn't find Ginga Common Core Component Manager headers.
Please install gingacc-cm project.

]))


###############
# Ginga-CC IC #
###############
AC_CHECK_HEADER([ic/IInteractiveChannelManager.h], [], AC_MSG_ERROR([

Couldn't find Ginga Common Core Interactive Channel headers.
Please install gingacc-ic project.

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

AC_CONFIG_FILES(
    Makefile include/um/Makefile src/Makefile test/Makefile)

AC_OUTPUT
