include $(top_srcdir)/build-aux/Makefile.am.common

################################## Ginga ###################################

AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_GINGACC_PLAYER)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_GINGA_NCL)\
  $(INCLUDE_GINGA_LSSM)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = -static -R $(gingalibdir) -rpath $(gingalibdir) $(PTHREAD_LIBS)

if BUILD_COMPONENTS
LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-cm/libgingacccm.la\
  $(NULL)

else
LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-ic/libgingacciccurlic.la\
  $(top_builddir)/gingacc-ic/libgingaccic.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlap.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlf.la\
  $(top_builddir)/gingacc-mb/libgingaccmb.la\
  $(top_builddir)/gingacc-mb/libgingaccmbsdl.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlimg.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlav.la\
  $(top_builddir)/gingacc-contextmanager/libgingacccontextmanager.la\
  $(top_builddir)/gingacc-player/libgingaccplacha.la\
  $(top_builddir)/gingacc-player/libgingaccplapav.la\
  $(top_builddir)/gingacc-player/libgingaccplalua.la\
  $(top_builddir)/gingacc-player/libgingaccplatxt.la\
  $(top_builddir)/gingacc-player/libgingaccplawdg.la\
  $(top_builddir)/gingacc-player/libgingaccplaavp.la\
  $(top_builddir)/gingacc-player/libgingaccplasrt.la\
  $(top_builddir)/gingacc-player/libgingaccplayer.la\
  $(top_builddir)/gingacc-player/libgingaccplaimg.la\
  $(top_builddir)/gingacc-player/libgingaccplaplt.la\
  $(top_builddir)/ncl30/libncl30.la\
  $(top_builddir)/ncl30-converter/libncl30connclncm.la\
  $(top_builddir)/ncl30-converter/libncl30confraxml.la\
  $(top_builddir)/ncl30-converter/libncl30confrancl.la\
  $(top_builddir)/gingancl/libginganclsubpadapter.la\
  $(top_builddir)/gingancl/libginganclluapadapter.la\
  $(top_builddir)/gingancl/libgingancladaptation.la\
  $(top_builddir)/gingancl/libginganclimgpadapter.la\
  $(top_builddir)/gingancl/libginganclpltpadapter.la\
  $(top_builddir)/gingancl/libginganclapppadapter.la\
  $(top_builddir)/gingancl/libginganclnclpadapter.la\
  $(top_builddir)/gingancl/libginganclfocus.la\
  $(top_builddir)/gingancl/libginganclemconverter.la\
  $(top_builddir)/gingancl/libginganclpavpadapter.la\
  $(top_builddir)/gingancl/libginganclmodel.la\
  $(top_builddir)/gingancl/libgingancl.la\
  $(top_builddir)/gingancl/libginganclprefetch.la\
  $(top_builddir)/gingancl/libgingancladapter.la\
  $(top_builddir)/gingancl/libginganclmd.la\
  $(top_builddir)/gingancl/libginganclchapadapter.la\
  $(top_builddir)/gingancl/libginganclaavpadapter.la\
  $(top_builddir)/gingalssm/libgingalssm.la\
  $(NULL)

if BUILD_DATA_PROCESSING
LDADD +=\
  $(top_builddir)/gingacc-tuner/libgingacctuner.la\
  $(top_builddir)/gingacc-tsparser/libgingacctsparser.la\
  $(top_builddir)/gingacc-dataprocessing/libgingaccdataprocncledit.la\
  $(top_builddir)/gingacc-dataprocessing/libgingaccdataprocessing.la\
  $(top_builddir)/gingacc-dataprocessing/libgingaccdataprocncl.la\
  $(NULL)
endif

if BUILD_MULTIDEVICE
LDADD +=\
  $(top_builddir)/gingacc-multidevice/libgingaccmd.la\
  $(NULL)
endif

if BUILD_BERKELIUM
LDADD +=\
  $(top_builddir)/gingancl/libginganclbklpadapter.la\
  $(top_builddir)/gingacc-player/libgingaccplabkl.la\
  $(NULL)
endif

endif

bin_PROGRAMS = ginga
ginga_SOURCES = src/main.cpp
