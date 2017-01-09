include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS= $(INCLUDE_GINGACC_SYSTEM)
AM_CXXFLAGS= $(CURL_CFLAGS)
AM_LDFLAGS= $(LTSTATIC) $(CURL_LIBS)

gingacciclib_LTLIBRARIES= libgingaccic.la
libgingaccic_la_SOURCES=\
  InteractiveChannelManager.cpp\
  CurlInteractiveChannel.cpp\
  $(NULL)
