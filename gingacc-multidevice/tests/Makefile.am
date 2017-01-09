include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = -static

LDADD =\
  $(LIBADD_TELEMIDIA_UTIL)\
  $(LIBADD_GINGACC_SYSTEM)\
  $(LIBADD_GINGACC_MB)\
  $(LIBADD_GINGACC_CONTEXTMANAGER)\
  $(LIBADD_GINGACC_MULTIDEVICE)\
  $(NULL)

# *** TESTS ARE BROKEN! ***
# noinst_PROGRAMS = test test-active
# test_SOURCES = test.cpp
# test_active_SOURCES = test-active.cpp
