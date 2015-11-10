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
AM_LDFLAGS = -R $(gingalibdir) -rpath $(gingalibdir) $(PTHREAD_LIBS) $(FFMPEG_LIBS)

if BUILD_COMPONENTS
LDADD =\
  ../telemidia-util/libtelemidiautil.la\
  ../gingacc-system/libgingaccsystem.la\
  ../gingacc-cm/libgingacccm.la\
  $(NULL)

else
LDADD =\
  ../gingacc-contextmanager/libgingacccontextmanager.la\
  ../gingacc-ic/libgingaccic.la\
  ../gingacc-ic/libgingacciccurlic.la\
  ../gingacc-mb/libgingaccmb.la\
  ../gingacc-mb/libgingaccmbdecsdlap.la\
  ../gingacc-mb/libgingaccmbdecsdlav.la\
  ../gingacc-mb/libgingaccmbdecsdlf.la\
  ../gingacc-mb/libgingaccmbdecsdlimg.la\
  ../gingacc-mb/libgingaccmbsdl.la\
  ../gingacc-player/libgingaccplaavp.la\
  ../gingacc-player/libgingaccplacha.la\
  ../gingacc-player/libgingaccplaimg.la\
  ../gingacc-player/libgingaccplalua.la\
  ../gingacc-player/libgingaccplapav.la\
  ../gingacc-player/libgingaccplaplt.la\
  ../gingacc-player/libgingaccplasrt.la\
  ../gingacc-player/libgingaccplatim.la\
  ../gingacc-player/libgingaccplatxt.la\
  ../gingacc-player/libgingaccplawdg.la\
  ../gingacc-player/libgingaccplayer.la\
  ../gingacc-system/libgingaccsystem.la\
  ../gingalssm/libgingalssm.la\
  ../gingancl/libgingancl.la\
  ../gingancl/libginganclaavpadapter.la\
  ../gingancl/libgingancladaptation.la\
  ../gingancl/libgingancladapter.la\
  ../gingancl/libginganclapppadapter.la\
  ../gingancl/libginganclchapadapter.la\
  ../gingancl/libginganclemconverter.la\
  ../gingancl/libginganclimgpadapter.la\
  ../gingancl/libginganclluapadapter.la\
  ../gingancl/libginganclmd.la\
  ../gingancl/libginganclmodel.la\
  ../gingancl/libginganclnclpadapter.la\
  ../gingancl/libginganclpavpadapter.la\
  ../gingancl/libginganclpltpadapter.la\
  ../gingancl/libginganclprefetch.la\
  ../gingancl/libginganclsubpadapter.la\
  ../gingancl/libgingancltimpadapter.la\
  ../ncl30-converter/libncl30confrancl.la\
  ../ncl30-converter/libncl30confraxml.la\
  ../ncl30-converter/libncl30connclncm.la\
  ../ncl30/libncl30.la\
  ../telemidia-util/libtelemidiautil.la\
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
LDADD +=\
  ../gingacc-multidevice/libgingaccmd.la\
  $(NULL)
endif

if BUILD_BERKELIUM
LDADD +=\
  ../gingancl/libginganclbklpadapter.la\
  ../gingacc-player/libgingaccplabkl.la\
  $(NULL)
endif

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
ginga_SOURCES = src/main.cpp
