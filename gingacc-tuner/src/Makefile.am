
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =
libdir = $(prefix)/lib/ginga
lib_LTLIBRARIES = libgingacctuner.la

TUNERCPPS = providers/Channel.cpp \
    providers/MulticastProvider.cpp \
	providers/UnicastProvider.cpp providers/FileSystemProvider.cpp \
	NetworkInterface.cpp Tuner.cpp

if V4LFRONTEND
TUNERCPPS += providers/frontends/isdbt/ISDBTFrontend.cpp providers/frontends/isdbt/ISDBTProvider.cpp providers/frontends/isdbt/RingBuffer.c
endif

libgingacctuner_la_SOURCES = $(TUNERCPPS)

libgingacctuner_la_LIBADD = -lpthread -lgingaccsystem -ltelemidiautil -lgingaccmb

libgingacctuner_la_LDFLAGS = \
                             -no-undefined \
                             -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
