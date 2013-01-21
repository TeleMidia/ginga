AC_INIT(gingacc-contextmanager, 0.13.5)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib/ /usr/local/lib/ ${prefix}/lib/ginga/; do
if test -d ${spfx}; then
LDFLAGS="-L${spfx} $LDFLAGS"
fi
done

for spfx in /usr/include/ /usr/local/include/ ${prefix}/include/ginga/; do
if test -d ${spfx}; then
CPPFLAGS="-I${spfx} $CPPFLAGS"
fi
done

#TODO: Search telemidia-util, gingacc-system ... include paths 
CPPFLAGS='-I${top_srcdir}'"/../gingacc-system/include $CPPFLAGS"
CPPFLAGS='-I${top_srcdir}'"/../../telemidia-util/include $CPPFLAGS"

###########
# HEADERS #
###########

#######
# LUA #
#######
AC_CHECK_HEADER([lua.h], [], AC_MSG_ERROR([

Couldn't find Lua Engine headers.
Please install Lua lib.

]))

#######################
# Ginga CC System I/O #
#######################
AC_CHECK_HEADER([mb/interface/IWindow.h], [], AC_MSG_ERROR([

Couldn't find Ginga Common Core System I/O headers.
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
   COMPONENTS_LIBS='-lgingaccsystem -lgingaccmb \
    -L/usr/local/lib/ -L${prefix}/lib/ginga/ -L${prefix}/lib/ginga/mb'
   TEST_COMPONENTS_LIBS='-lgingaccmb \
    -lgingaccsystem -lgingacccontextmanager \
    -L/usr/local/lib/ -L${prefix}/lib/ginga/ -L${prefix}/lib/ginga/mb'
   AC_DEFINE(HAVE_COMPSUPPORT, 0, [Def to 0 if you don't have Component support.])
   AC_MSG_WARN([*** Component support will not be built.])

else
   COMPONENTS_LIBS='-ldl -L${prefix}/lib/ginga/'
   TEST_COMPONENTS_LIBS='-ldl -L${prefix}/lib/ginga/'
   AC_DEFINE(HAVE_COMPSUPPORT, 1, [Def to 1 if you have Component support.])
   AC_MSG_RESULT([*** Component support will be built.])
fi

AM_CONDITIONAL([COMPONENTS], [test x$enable_components = xyes])
AC_SUBST(COMPONENTS_LIBS)
AC_SUBST(TEST_COMPONENTS_LIBS)


AC_CONFIG_FILES(
    Makefile 
    files/Makefile 
    include/contextmanager/Makefile
    include/contextmanager/system/Makefile
    include/contextmanager/user/Makefile
    src/Makefile 
    test/Makefile)

AC_OUTPUT

AC_MSG_RESULT([
Build options: 
  Module directory                 $MODULEDIR
  Component Support                $enable_components
])
