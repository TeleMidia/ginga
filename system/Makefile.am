include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  -DSTR_PREFIX='"$(prefix)"'\
  -DGINGA_LIBDIR='"$(gingalibdir)"'\
  -DGINGA_INI_PATH='"$(gingaconfdir)/ginga.ini"'\
  $(NULL)

AM_CXXFLAGS=
AM_LDFLAGS= $(LTSTATIC)
if BUILD_MULTIDEVICE
AM_CXXFLAGS+= $(LIBZIP_CFLAGS)
AM_LDFLAGS+= $(LIBZIP_LIBS)
endif
noinst_LTLIBRARIES= system.la
system_la_SOURCES=\
  Thread.cpp\
  GingaLocatorFactory.cpp\
  PracticalSocket.cpp\
  SystemCompat.cpp\
  Stc.cpp\
  $(NULL)
