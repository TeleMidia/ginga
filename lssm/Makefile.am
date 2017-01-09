include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_GINGACC_PLAYER)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_GINGA_NCL)\
  $(NULL)

AM_LDFLAGS= $(LTSTATIC)

noinst_LTLIBRARIES= libgingalssm.la
libgingalssm_la_SOURCES=\
  PresentationEngineManager.cpp\
  CommonCoreManager.cpp\
  DataWrapperListener.cpp\
  StcWrapper.cpp\
  $(NULL)

if BUILD_DATA_PROCESSING
libgingalssm_la_LIBADD=\
  ../gingacc-tuner/libgingacctuner.la\
  ../gingacc-tsparser/libgingacctsparser.la\
  ../gingacc-dataprocessing/libgingaccdataprocncledit.la\
  ../gingacc-dataprocessing/libgingaccdataprocessing.la\
  ../gingacc-dataprocessing/libgingaccdataprocncl.la\
  ../ncl30/libncl30.la\
  $(NULL)
endif
