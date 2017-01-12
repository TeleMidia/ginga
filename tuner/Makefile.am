include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS+=\
  -DGINGA_ISDBT_PATH='"$(gingaconfdir)/tuner/isdbt.ini"'\
  -DGINGA_TUNER_PATH='"$(gingaconfdir)/tuner/tuner.ini"'\
  $(NULL)

AM_CXXFLAGS=
AM_LDFLAGS= $(LTSTATIC)

noinst_LTLIBRARIES= tuner.la
tuner_la_SOURCES=\
  MulticastProvider.cpp\
  Channel.cpp\
  UnicastProvider.cpp\
  FileSystemProvider.cpp\
  NetworkProvider.cpp\
  NetworkInterface.cpp\
  Tuner.cpp\
  $(NULL)

if BUILD_LINUXDVB
tuner_la_SOURCES+=\
  ISDBTFrontend.cpp\
  ISDBTProvider.cpp\
  RingBuffer.c\
  $(NULL)
endif
