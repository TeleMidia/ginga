include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_TUNER)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = -static

LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-mb/libgingaccmb.la\
  $(top_builddir)/gingacc-tuner/tuner.la\
  $(NULL)

noinst_PROGRAMS = test
test_SOURCES = test.cpp
