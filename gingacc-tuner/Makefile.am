include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  -DGINGA_ISDBT_PATH='"$(gingaconfdir)/tuner/isdbt.ini"'\
  -DGINGA_TUNER_PATH='"$(gingaconfdir)/tuner/tuner.ini"'\
  $(INCLUDE_GINGACC_TUNER)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS)

gingacctunerlib_LTLIBRARIES = libgingacctuner.la

libgingacctuner_la_SOURCES =\
  src/providers/MulticastProvider.cpp\
  src/providers/Channel.cpp\
  src/providers/UnicastProvider.cpp\
  src/providers/FileSystemProvider.cpp\
  src/providers/NetworkProvider.cpp\
  src/NetworkInterface.cpp\
  src/Tuner.cpp\
  $(NULL)

if BUILD_LINUXDVB
libgingacctuner_la_SOURCES +=\
  src/providers/frontends/isdbt/ISDBTFrontend.cpp\
  src/providers/frontends/isdbt/ISDBTProvider.cpp\
  src/providers/frontends/isdbt/RingBuffer.c\
  $(NULL)
endif
