include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS)

gingacccontextmanagerlib_LTLIBRARIES = libgingacccontextmanager.la

libgingacccontextmanager_la_SOURCES =\
  src/ContextManager.cpp\
  src/user/GingaUser.cpp\
  src/system/SystemInfo.cpp\
  $(NULL)
