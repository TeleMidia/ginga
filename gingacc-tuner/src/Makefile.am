
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =
localLibdir = /usr/local/lib/ginga
localLib_PROGRAMS = libgingacctuner.so

TUNERCPPS = providers/Channel.cpp \
    providers/MulticastProvider.cpp \
	providers/UnicastProvider.cpp providers/FileSystemProvider.cpp \
	NetworkInterface.cpp Tuner.cpp

if V4LFRONTEND
TUNERCPPS += providers/frontends/isdbt/ISDBTFrontend.cpp providers/frontends/isdbt/ISDBTProvider.cpp
endif

libgingacctuner_so_SOURCES = $(TUNERCPPS)

libgingacctuner_so_LDADD = -lpthread -lgingaccsystem -ltelemidiautil

libgingacctuner_so_LDFLAGS = -L/usr/local/lib/ginga -L/usr/local/lib \
	-g -shared -Wl,-soname,libgingacctuner.so

MAINTAINERCLEANFILES = Makefile.in
