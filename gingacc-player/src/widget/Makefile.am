
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libgingaccplawdgdir = /usr/local/lib/ginga/players/

libgingaccplawdg_so_LDFLAGS = -Wl,-soname,libgingaccplawdg.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../ -L../image

libgingaccplawdg_PROGRAMS = libgingaccplawdg.so

libgingaccplawdg_so_SOURCES = ShowButton.cpp

libgingaccplawdg_so_LDADD = -lgingaccsystem -lgingaccplayer \
    -lgingaccplaimg $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
