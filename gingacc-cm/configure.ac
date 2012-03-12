AC_INIT(gingacc-cm, 0.13.1)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
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
