include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_IC)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS) $(CURL_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS) $(CURL_LIBS)

gingacciclib_LTLIBRARIES =\
  libgingaccic.la\
  $(NULL)

libgingaccic_la_SOURCES =\
  src/InteractiveChannelManager.cpp\
  src/curlic/CurlInteractiveChannel.cpp\
  $(NULL)
