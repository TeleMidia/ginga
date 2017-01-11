include $(top_srcdir)/build-aux/Makefile.am.common

if BUILD_DATA_PROCESSING
AM_CPPFLAGS=\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)
endif

AM_LDFLAGS= $(LTSTATIC)
noinst_LTLIBRARIES= lssm.la
lssm_la_SOURCES=\
  PresentationEngineManager.cpp\
  CommonCoreManager.cpp\
  DataWrapperListener.cpp\
  StcWrapper.cpp\
  $(NULL)
