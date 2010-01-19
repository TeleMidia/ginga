
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

LIBS =

#########
# IMAGE #
#########
libgingaccplaimgdir = /usr/local/lib/ginga/players/

libgingaccplaimg_so_LDFLAGS = -Wl,-soname,libgingaccplaimg.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../

libgingaccplaimg_PROGRAMS = libgingaccplaimg.so

libgingaccplaimg_so_SOURCES = ImagePlayer.cpp

libgingaccplaimg_so_LDADD = -ltelemidiautil -lgingaccplayer $(COMPONENTS_LIBS)

#########
# ANIME #
#########
libgingaccplaanidir = /usr/local/lib/ginga/players/

libgingaccplaani_so_LDFLAGS = -Wl,-soname,libgingaccplaani.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../

libgingaccplaani_PROGRAMS = libgingaccplaani.so

libgingaccplaani_so_SOURCES = AnimePlayer.cpp

libgingaccplaani_so_LDADD = -ltelemidiautil -lgingaccsystemthread \
    $(COMPONENTS_LIBS)
