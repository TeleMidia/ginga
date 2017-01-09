include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_MB)\
  $(NULL)

AM_LDFLAGS= $(LTSTATIC)
noinst_LTLIBRARIES= ctxmgmt.la
ctxmgmt_la_SOURCES=\
  ContextManager.cpp\
  GingaUser.cpp\
  SystemInfo.cpp\
  $(NULL)
