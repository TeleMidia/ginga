include $(top_srcdir)/build-aux/Makefile.am.common

AM_LDFLAGS = -R $(gingalibdir) -rpath $(gingalibdir) $(PTHREAD_LIBS) $(FFMPEG_LIBS)

LDADD=\
  ../ctxmgmt/ctxmgmt.la\
  ../ic/ic.la\
  ../mb/mb.la\
  ../system/system.la\
  ../lssm/lssm.la\
  ../gingancl/gingancl.la\
  ../player/player.la\
  ../nclconv/nclconv.la\
  ../ncl/ncl.la\
  $(NULL)

if BUILD_ISDBT
LDADD+=\
  ../tuner/tuner.la\
  ../tsparser/tsparser.la\
  ../dataprocessing/dataprocessing.la\
  $(NULL)
endif

if BUILD_MULTIDEVICE
LDADD+= ../multidevice/multidevice.la
endif

bin_PROGRAMS = ginga
ginga_SOURCES= $(sources)
sources=
sources+= main.cpp
sources+= util/Base64.cpp
sources+= util/Color.cpp
sources+= util/functions.cpp
