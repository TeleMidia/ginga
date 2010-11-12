
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libgingaccplatxtdir = /usr/local/lib/ginga/players/

libgingaccplatxt_so_LDFLAGS = -Wl,-soname,libgingaccplatxt.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../

libgingaccplatxt_PROGRAMS = libgingaccplatxt.so

libgingaccplatxt_so_SOURCES = TextPlayer.cpp

libgingaccplatxt_so_LDADD = -ltelemidiautil -lgingaccplayer $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
