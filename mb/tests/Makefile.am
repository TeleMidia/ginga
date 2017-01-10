include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = -static

LDADD =\
	-lboost_system\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-mb/libgingaccmb.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlap.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlav.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlf.la\
  $(top_builddir)/gingacc-mb/libgingaccmbdecsdlimg.la\
  $(top_builddir)/gingacc-mb/libgingaccmbsdl.la\
  $(NULL)

noinst_PROGRAMS =\
  simpleTestImage\
  simpleTestInput\
  simpleTestText\
  simpleTestVideo\
  $(NULL)

simpleTestImage_SOURCES = simpleTestImage.cpp
simpleTestInput_SOURCES = simpleTestInput.cpp
simpleTestText_SOURCES = simpleTestText.cpp
simpleTestVideo_SOURCES = simpleTestVideo.cpp
