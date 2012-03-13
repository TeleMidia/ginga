
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/players

if LINKSLIB
lib_LTLIBRARIES = libgingaccplalks.la

libgingaccplalks_la_SOURCES = LinksPlayer.cpp

libgingaccplalks_la_LIBADD = -ltelemidialinks -ltelemidiautil -lgingaccplayer \
    -lgingaccsystem -lpthread $(COMPONENTS_LIBS) \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../../../../
endif
MAINTAINERCLEANFILES = Makefile.in
