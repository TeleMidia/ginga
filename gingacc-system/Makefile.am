include $(top_srcdir)/build-aux/Makefile.am.common

############################## gingacc-system ##############################
AM_CPPFLAGS =\
  -DSTR_PREFIX='"$(prefix)"'\
  -DGINGA_LIBDIR='"$(gingalibdir)"'\
  -DGINGA_INI_PATH='"$(gingaconfdir)/ginga.ini"'\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LIBS) $(DL_LIBS)

gingaccsystemlib_LTLIBRARIES = libgingaccsystem.la

libgingaccsystem_la_SOURCES =\
  src/thread/Thread.cpp\
  src/fs/GingaLocatorFactory.cpp\
  src/compat/PracticalSocket.cpp\
  src/compat/SystemCompat.cpp\
  src/time/Stc.cpp\
  $(NULL)

if BUILD_MULTIDEVICE
AM_CXXFLAGS += $(LIBZIP_CFLAGS)
AM_LDFLAGS += $(LIBZIP_LIBS)
endif
