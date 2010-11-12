
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libgingaccplalksdir = /usr/local/lib/ginga/players/

libgingaccplalks_so_LDFLAGS = -Wl,-soname,libgingaccplalks.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../../

libgingaccplalks_PROGRAMS = libgingaccplalks.so

libgingaccplalks_so_SOURCES = LinksPlayer.cpp

libgingaccplalks_so_LDADD = -ltelemidialinks -ltelemidiautil -lgingaccplayer \
    -lgingaccsystemthread -lpthread $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
