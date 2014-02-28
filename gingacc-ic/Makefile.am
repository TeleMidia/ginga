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
  libgingacciccurlic.la\
  $(NULL)

# gingacc-ic
libgingaccic_la_SOURCES =\
  src/InteractiveChannelManager.cpp\
  $(NULL)

# gingacc-ic-curlic
libgingacciccurlic_la_SOURCES =\
  src/curlic/CurlInteractiveChannel.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
gingaccicinclude_HEADERS =\
  include/ic/IInteractiveChannel.h\
  include/ic/IInteractiveChannelListener.h\
  include/ic/IInteractiveChannelManager.h\
  include/ic/InteractiveChannelManager.h\
  $(NULL)
endif
