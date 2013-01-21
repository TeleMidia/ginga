AC_INIT(gingaccsystem, 0.13.5)

AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_LANG_CPLUSPLUS
AC_PROG_CXX([g++])
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

# Bring additional directories where things might be found into our
# search path. I don't know why autoconf doesn't do this by default
for spfx in /usr/lib /usr/local/lib ${prefix}/lib/ginga; do
if test -d ${spfx}; then
LDFLAGS="-L${spfx} $LDFLAGS"
fi
done

for spfx in /usr/include /usr/local/include ${prefix}/include/ginga; do
if test -d ${spfx}; then
CPPFLAGS="-I${spfx} $CPPFLAGS"
fi
done

###########
# HEADERS #
###########

#########################
# Linux and Mac OS 10.6 #
#########################

AC_CHECK_HEADERS([sys/socket.h])
AC_CHECK_HEADERS([net/if.h])

##############
# Linux only #
##############

AC_CHECK_HEADERS([sys/sysinfo])


####################
# Mac OS 10.6 only #
####################

AC_CHECK_HEADERS([mach/mach.h] [mach/mach_host.h] [mach/mach_init.h] [mach/mach_types.h] [mach/vm_statistics.h] [sys/sysctl.h],
		 [AC_DEFINE([HAVE_MACH_SL_H], [1],
			    [Define to 1 if you have <mach/mach.h>, <mach/mach_host.h>, <mach/mack_init.h>, <mach/mach_types.h>, <mach/vm_statistics.h> and <sys/sysctl.h>])],
		 [AC_MSG_ERROR([

				Couldn't find <mach/mach.h>, <mac/mach_host.h>, <mach/mack_init.h>, <mach/mach_types.h>, <mach/vm_statistics.h> and <sys/sysctl.h>.
				Please install Mac OS SDK headers.
			       ])],
			      [])


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

#######################
# Dynamic Library Lib #
#######################

DL_LIBS='-L./'

AC_HAVE_LIBRARY(dl, DL_LIBS='-ldl', AC_MSG_WARN([Couldn't find libdl.]))
AC_SUBST(DL_LIBS)

###########################
# Multple Process Support #
###########################
AC_HAVE_LIBRARY(
    rt,
    multiprocess=yes,
    AC_MSG_WARN([Couldn't find librt.])
    multiprocess=no
)

if test x$multiprocess = xno; then
   MULTIPROCESS_LIBS='-L${prefix}/lib/ginga/'
   AC_DEFINE(HAVE_MULTIPROCESS, 0, [Def to 0 if you don't have Multiple Process support.])
   AC_MSG_WARN([*** Multiple process support will not be built.])
else
   MULTIPROCESS_LIBS='-lrt -lpthread -ltelemidiautil -L${prefix}/lib/ginga/'
   AC_DEFINE(HAVE_MULTIPROCESS, 1, [Def to 1 if you have Multiple Process support.])
   AC_MSG_RESULT([*** Multiple Process support will be built.])
fi

AM_CONDITIONAL([MULTIPROCESS], [test x$multiprocess = xyes])
AC_SUBST(MULTIPROCESS_LIBS)

AC_CONFIG_FILES(
	Makefile
	include/system/thread/Makefile
	include/system/compat/Makefile
	include/system/fs/Makefile
	include/system/time/Makefile
	include/system/process/Makefile 
	src/Makefile
	test/Makefile
)

AC_OUTPUT

AC_MSG_RESULT([
Build options: 
  Module directory                 $MODULEDIR
  Multi Process Support            $multiprocess
])
