AC_INIT(gingacc-ic, 0.13.2)

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

#TODO: Search telemidia-util include path 
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
# Ginga-CC System #
###################
AC_CHECK_HEADER([system/thread/Thread.h], [], AC_MSG_ERROR([

Couldn't find Ginga-CC System Thread headers.
Please install gingacc-system project.

]))

################
# CURL Support #
################
curl=yes
AC_ARG_ENABLE(curl,
  [  --enable-curl build project with curl ic [[default=yes]]],
  enable_curl="$enableval", enable_curl=yes)

AC_CHECK_HEADER(
  [curl/curl.h],
  [],
  AC_MSG_WARN([*** CURL header not Found.])
    enable_curl=no
)

if test x$enable_curl = xno; then
   AC_MSG_WARN([*** Ginga CURL IC support will not be built.])
   AC_DEFINE(HAVE_CURL, 0, [Def to 0 if you don't have ginga curl support.])
   CURL_LIBS='-L/usr/local/lib/ginga'

else
   AC_MSG_RESULT([*** Ginga CURL IC support will be built.])
   AC_DEFINE(HAVE_CURL, 1, [Def to 1 if you have ginga curl support.])
   CURL_LIBS='-lgingacciccurlic -L/usr/local/lib/ginga/'
fi

AM_CONDITIONAL([CURLLIB], [test x$enable_curl = xyes])

#################
# CCRTP Support #
#################
ccrtp=yes
AC_ARG_ENABLE(ccrtp,
  [  --enable-ccrtp build project with ccrtp ic [[default=yes]]],
  enable_ccrtp="$enableval", enable_ccrtp=yes)

AC_CHECK_HEADER(
  [ccrtp/rtp.h],
  [],
  AC_MSG_WARN([*** CCRTP header not Found.])
    enable_ccrtp=no
)

if test x$enable_ccrtp = xno; then
   AC_MSG_WARN([*** Ginga CCRTP IC support will not be built.])
   AC_DEFINE(HAVE_CCRTP, 0, [Def to 0 if you don't have ginga ccrtp support.])
   CCRTP_LIBS='-L/usr/local/lib/ginga/'

else
   AC_MSG_RESULT([*** Ginga CCRTP IC support will be built.])
   AC_DEFINE(HAVE_CCRTP, 1, [Def to 1 if you have ginga ccrtp support.])
   CCRTP_LIBS='-lgingaccicccrtp -L/usr/local/lib/ginga/'
fi

AM_CONDITIONAL([CCRTPLIB], [test x$enable_ccrtp = xyes])

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
   COMPONENTS_LIBS='-L/usr/local/lib/ -L/usr/local/lib/ginga/ -lgingaccsystem'
   TEST_COMPONENTS_LIBS='-lgingaccic -L/usr/local/lib/ -L/usr/local/lib/ginga/ -lgingaccsystem'
   AC_DEFINE(HAVE_COMPSUPPORT, 0, [Def to 0 if you don't have Component support.])
   AC_MSG_WARN([*** Component support will not be built.])

else
   COMPONENTS_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   TEST_COMPONENTS_LIBS='-ldl -L/usr/local/lib/ -L/usr/local/lib/ginga/'
   AC_DEFINE(HAVE_COMPSUPPORT, 1, [Def to 1 if you have Component support.])
   AC_MSG_RESULT([*** Component support will be built.])
fi

AM_CONDITIONAL([COMPONENTS], [test x$enable_components = xyes])

AC_SUBST(CURL_LIBS)
AC_SUBST(CCRTP_LIBS)
AC_SUBST(COMPONENTS_LIBS)
AC_SUBST(ICM_COMPONENTS_LIBS)
AC_SUBST(TEST_COMPONENTS_LIBS)

AC_CONFIG_FILES(
    Makefile
    include/ic/Makefile
    src/ccrtpic/Makefile 
    src/curlic/Makefile
    src/Makefile
    test/Makefile)

AC_OUTPUT

AC_MSG_RESULT([
Build options: 
  Module directory                      $MODULEDIR
  RTP Support                           $enable_ccrtp
  CURL Support                          $enable_curl
  Component Support                     $enable_components
])
