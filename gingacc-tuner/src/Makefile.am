
# set the include path found by configure
INCLUDES = $(all_includes)
LIBS =
localLibdir = /usr/local/lib/ginga
localLib_PROGRAMS = libgingacctuner.so

libgingacctuner_so_SOURCES = providers/Channel.cpp \
    providers/MulticastProvider.cpp \
	providers/UnicastProvider.cpp providers/FileSystemProvider.cpp \
	providers/frontends/isdbt/ISDBTFrontend.cpp \
	providers/frontends/isdbt/ISDBTProvider.cpp \
	NetworkInterface.cpp Tuner.cpp

libgingacctuner_so_LDADD = -lpthread -lgingaccsystemthread -ltelemidiautil

libgingacctuner_so_LDFLAGS = -L/usr/local/lib/ginga -L/usr/local/lib \
	-g -shared -Wl,-soname,libgingacctuner.so
