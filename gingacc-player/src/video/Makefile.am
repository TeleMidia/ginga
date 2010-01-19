
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

LIBS =

localLibdir = /usr/local/lib/ginga/players/

libgingaccplaavp_so_LDFLAGS = -Wl,-soname,libgingaccplaavp.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../

localLib_PROGRAMS = libgingaccplaavp.so

libgingaccplaavp_so_SOURCES = AVPlayer.cpp

libgingaccplaavp_so_LDADD = -ltelemidiautil -lgingaccsystemthread \
    -lgingaccplayer $(COMPONENTS_LIBS)
