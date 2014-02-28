include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_IC)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = -static

LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-ic/libgingaccic.la\
  $(top_builddir)/gingacc-ic/libgingacciccurlic.la\
  $(NULL)

noinst_PROGRAMS = test
test_SOURCES = test.cpp
