include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_PLAYER)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = -static

LDADD =\
  $(top_builddir)/telemidia-util/libtelemidiautil.la\
  $(top_builddir)/gingacc-system/libgingaccsystem.la\
  $(NULL)

noinst_PROGRAMS =\
  testAudio\
  testImage\
  testText\
  testVideo\
  $(NULL)

testAudio_SOURCES = testAudio.cpp
testImage_SOURCES = testImage.cpp
testText_SOURCES = testText.cpp
testVideo_SOURCES = testVideo.cpp
