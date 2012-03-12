
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/players

lib_LTLIBRARIES = libgingaccplatxt.la

libgingaccplatxt_la_SOURCES = TextPlayer.cpp

libgingaccplatxt_la_LIBADD = -ltelemidiautil -lgingaccplayer \
$(COMPONENTS_LIBS) -L/usr/local/lib -L/usr/local/lib/ginga -L../

MAINTAINERCLEANFILES = Makefile.in
