AC_INIT(gingacc-cm, 0.13.5)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib/ /usr/local/lib/ /usr/local/lib/ginga/; do
if test -d ${spfx}; then
LDFLAGS="-L${spfx} $LDFLAGS"
fi
done

for spfx in /usr/include/ /usr/local/include/ /usr/local/include/ginga/; do
if test -d ${spfx}; then
CPPFLAGS="-I${spfx} $CPPFLAGS"
fi
done

###########
# HEADERS #
###########

#########
# Expat #
#########
AC_CHECK_HEADER([expat.h], [], AC_MSG_ERROR([

Couldn't find expat headers.
Please install libexpat.

]))

##################
# gingacc-system #
##################
AC_CHECK_HEADER([system/compat/SystemCompat.h], [], AC_MSG_ERROR([

Couldn't find gingacc-system headers.
Please install gingacc-system.

]))

AC_CONFIG_FILES(
    Makefile
    include/cm/Makefile 
    include/cm/component/Makefile 
    include/cm/assembler/Makefile
    include/cm/parser/Makefile
    files/Makefile
    src/Makefile
    src/component/Makefile
    src/assembler/Makefile
    src/parser/Makefile
    test/Makefile)

AC_OUTPUT
