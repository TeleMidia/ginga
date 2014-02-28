include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = -static

LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(top_builddir)/gingacc-mb/libgingaccmb.la\
  $(NULL)

noinst_PROGRAMS =\
  testAudio\
  testFont\
  testInput\
  testScreen\
  testSingleVideo\
  testVideo\
  $(NULL)

testAudio_SOURCES = testAudio.cpp
testFont_SOURCES = testFont.cpp
testInput_SOURCES = testInput.cpp
testScreen_SOURCES = testScreen.cpp
testSingleVideo_SOURCES = testSingleVideo.cpp
testVideo_SOURCES = testVideo.cpp
