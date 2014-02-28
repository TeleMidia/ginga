include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_IC)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_GINGACC_PLAYER)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_GINGA_NCL)\
  $(INCLUDE_GINGA_LSSM)\
  $(NULL)

if BUILD_DATA_PROCESSING
AM_CPPFLAGS +=\
  $(INCLUDE_GINGACC_TUNER)\
  $(INCLUDE_GINGACC_TSPARSER)\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)
endif

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LT_LDLFAGS) $(PTHREAD_LIBS)

gingalssmlib_LTLIBRARIES = libgingalssm.la

libgingalssm_la_SOURCES =\
  src/PresentationEngineManager.cpp\
  src/CommonCoreManager.cpp\
  src/DataWrapperListener.cpp\
  src/StcWrapper.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
gingalssminclude_HEADERS =\
  include/gingalssm/IPresentationEngineManager.h\
  include/gingalssm/ICommonCoreManager.h\
  $(NULL)
endif
