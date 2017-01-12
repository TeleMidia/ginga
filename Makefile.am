include $(top_srcdir)/build-aux/Makefile.am.common

ACLOCAL_AMFLAGS= -I build-aux ${ACLOCAL_FLAGS}

SUBDIRS=
SUBDIRS+= data
SUBDIRS+= ic
SUBDIRS+= mb
SUBDIRS+= ctxmgmt
if BUILD_MULTIDEVICE
SUBDIRS+= multidevice
endif
if BUILD_ISDBT
SUBDIRS+= tuner
SUBDIRS+= tsparser
SUBDIRS+= dataprocessing
endif
SUBDIRS+= player
SUBDIRS+= ncl
SUBDIRS+= nclconv
SUBDIRS+= system
SUBDIRS+= gingancl
SUBDIRS+= lssm
SUBDIRS+= src

if BUILD_TESTS
SUBDIRS+= mb/tests
SUBDIRS+= player/tests
endif

MAINTAINERCLEANFILES+=\
  INSTALL\
  Makefile.in\
  aclocal.m4\
  build-aux/ar-lib\
  build-aux/compile\
  build-aux/config.guess\
  build-aux/config.sub\
  build-aux/depcomp\
  build-aux/install-sh\
  build-aux/libtool.m4\
  build-aux/ltmain.sh\
  build-aux/ltoptions.m4\
  build-aux/ltsugar.m4\
  build-aux/ltversion.m4\
  build-aux/lt~obsolete.m4\
  build-aux/missing\
  config.h.in\
  config.h.in~\
  configure\
  $(NULL)
