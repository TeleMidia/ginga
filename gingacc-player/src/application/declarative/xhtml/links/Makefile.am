
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

if LINKSLIB
libgingaccplalksdir = /usr/local/lib/ginga/players/

libgingaccplalks_so_LDFLAGS = -Wl,-soname,libgingaccplalks.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../../../

libgingaccplalks_PROGRAMS = libgingaccplalks.so

libgingaccplalks_so_SOURCES = LinksPlayer.cpp

libgingaccplalks_so_LDADD = -ltelemidialinks -ltelemidiautil -lgingaccplayer \
    -lgingaccsystem -lpthread $(COMPONENTS_LIBS)
endif
MAINTAINERCLEANFILES = Makefile.in
