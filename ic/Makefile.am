include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  $(INCLUDE_GINGACC_SYSTEM)
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS) $(CURL_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS) $(CURL_LIBS)

gingacciclib_LTLIBRARIES =\
  libgingaccic.la\
  $(NULL)

libgingaccic_la_SOURCES =\
  InteractiveChannelManager.cpp\
  CurlInteractiveChannel.cpp\
  $(NULL)
