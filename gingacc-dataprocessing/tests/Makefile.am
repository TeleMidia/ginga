include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_TUNER)\
  $(INCLUDE_GINGACC_TSPARSER)\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = -static

LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-tsparser/libgingacctsparser.la\
  $(top_builddir)/gingacc-dataprocessing/libgingaccdataprocessing.la\
  $(top_builddir)/gingacc-dataprocessing/libgingaccdataprocncl.la\
  $(top_builddir)/gingacc-dataprocessing/libgingaccdataprocncledit.la\
  $(NULL)

noinst_PROGRAMS = test
test_SOURCES = test.cpp
