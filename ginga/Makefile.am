include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
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
  ../util/util.la\
  $(NULL)

if BUILD_DATA_PROCESSING
LDADD+=\
  ../tuner/tuner.la\
  ../gingacc-tsparser/tsparser.la\
  ../gingacc-dataprocessing/libgingaccdataprocncledit.la\
  ../gingacc-dataprocessing/libgingaccdataprocessing.la\
  ../gingacc-dataprocessing/libgingaccdataprocncl.la\
  $(NULL)
endif

if BUILD_MULTIDEVICE
LDADD+=\
  ../gingacc-multidevice/libgingaccmd.la\
  $(NULL)
endif

bin_PROGRAMS = ginga
ginga_SOURCES = main.cpp
