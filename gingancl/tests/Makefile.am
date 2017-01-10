include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_GINGA_NCL)\
  $(NULL)

AM_LDFLAGS = -static

LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-mb/libgingaccmb.la\
  $(top_builddir)/ncl30/libncl30.la\
  $(top_builddir)/gingancl/libgingancl.la\
  $(top_builddir)/gingancl/libginganclaavpadapter.la\
  $(top_builddir)/gingancl/libgingancladaptation.la\
  $(top_builddir)/gingancl/libgingancladapter.la\
  $(top_builddir)/gingancl/libginganclapppadapter.la\
  $(top_builddir)/gingancl/libginganclchapadapter.la\
  $(top_builddir)/gingancl/libginganclluapadapter.la\
  $(top_builddir)/gingancl/libginganclemconverter.la\
  $(top_builddir)/gingancl/libginganclimgpadapter.la\
  $(top_builddir)/gingancl/libginganclmd.la\
  $(top_builddir)/gingancl/libginganclmodel.la\
  $(top_builddir)/gingancl/libginganclnclpadapter.la\
  $(top_builddir)/gingancl/libginganclpavpadapter.la\
  $(top_builddir)/gingancl/libginganclpltpadapter.la\
  $(top_builddir)/gingancl/libginganclprefetch.la\
  $(top_builddir)/gingancl/libginganclsubpadapter.la\
  $(NULL)

if BUILD_MULTIDEVICE
LDADD +=\
  $(LIBADD_GINGACC_MULTIDEVICE)\
  $(top_builddir)/gingancl/libginganclmd.la\
  $(NULL)
endif

noinst_PROGRAMS = testFormatter
testFormatter_SOURCES = testFormatter.cpp
