
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
	$(all_includes)

LIBS =

localLibdir = /usr/local/lib/ginga
localLib_PROGRAMS = libgingaccplayer.so

libgingaccplayer_so_SOURCES = Player.cpp

libgingaccplayer_so_LDFLAGS = -L/usr/local/lib/ginga -L/usr/local/lib \
	-g -shared -Wl,-soname,libgingaccplayer.so

libgingaccplayer_so_LDADD = -lpthread -ltelemidiautil \
    -lgingaccsystemiocodemap $(COMPONENTS_LIBS)
