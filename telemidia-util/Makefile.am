include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS = $(INCLUDE_TELEMIDIA_UTIL)
AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LIBS)

telemidiautillib_LTLIBRARIES = libtelemidiautil.la

libtelemidiautil_la_SOURCES =\
  src/Base64.cpp\
  src/Color.cpp\
  src/functions.cpp\
  $(NULL)
