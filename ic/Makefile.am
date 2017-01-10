include $(top_srcdir)/build-aux/Makefile.am.common

AM_CXXFLAGS= $(CURL_CFLAGS)
AM_LDFLAGS= $(LTSTATIC) $(CURL_LIBS)
noinst_LTLIBRARIES= ic.la
ic_la_SOURCES=\
  InteractiveChannelManager.cpp\
  CurlInteractiveChannel.cpp\
  $(NULL)
