include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_GINGA_NCL)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = -R $(gingalibdir) -rpath $(gingalibdir) $(PTHREAD_LIBS) $(FFMPEG_LIBS)

LDADD=\
  ../ctxmgmt/ctxmgmt.la\
  ../ic/ic.la\
  ../mb/mb.la\
  ../system/system.la\
  ../lssm/lssm.la\
  ../gingancl/libginganclmodel.la\
  ../player/player.la\
  ../ncl30-converter/libncl30confrancl.la\
  ../ncl30/libncl30.la\
  ../util/util.la\
  $(NULL)

if BUILD_DATA_PROCESSING
LDADD +=\
  ../gingacc-tuner/libgingacctuner.la\
  ../gingacc-tsparser/libgingacctsparser.la\
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

if BUILD_BERKELIUM
LDADD+=\
  ../gingancl/libginganclbklpadapter.la\
  $(NULL)
endif

if BUILD_SSML
LDADD +=\
  ../gingancl/libginganclssmlpadapter.la\
  ../gingacc-player/libgingaccplassml.la\
  $(NULL)
endif

if BUILD_LIBBPG
AM_LDFLAGS += $(LIBBPG_LIBS)
endif

bin_PROGRAMS = ginga
ginga_SOURCES = main.cpp
