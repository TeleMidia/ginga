
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
	$(all_includes)

LIBS =

localLibdir = /usr/local/lib/ginga
localLib_PROGRAMS = libgingaccplayer.so

CPPSRCS = Player.cpp
if MULTIPROCESS
CPPSRCS += PlayerProcess.cpp
endif

libgingaccplayer_so_SOURCES = $(CPPSRCS)

libgingaccplayer_so_LDFLAGS = -L/usr/local/lib/ginga -L/usr/local/lib \
	-g -shared -Wl,-soname,libgingaccplayer.so

libgingaccplayer_so_LDADD = -lpthread -ltelemidiautil \
    -lgingaccsystemiocodemap $(MULTIPROCESS_LIBS) $(COMPONENTS_LIBS)
