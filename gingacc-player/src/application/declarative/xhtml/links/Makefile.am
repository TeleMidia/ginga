
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/players

if LINKSLIB
lib_LTLIBRARIES = libgingaccplalks.la

libgingaccplalks_la_SOURCES = LinksPlayer.cpp

libgingaccplalks_la_LIBADD = -ltelemidialinks -ltelemidiautil -lgingaccplayer \
    -lgingaccsystem -lpthread $(COMPONENTS_LIBS) \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../../../../
endif
MAINTAINERCLEANFILES = Makefile.in
