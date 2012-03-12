
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/players

lib_LTLIBRARIES = libgingaccplawdg.la

libgingaccplawdg_la_SOURCES = ShowButton.cpp

libgingaccplawdg_la_LIBADD = -lgingaccsystem -lgingaccplayer \
    -lgingaccplaimg $(COMPONENTS_LIBS) \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../ -L../image

MAINTAINERCLEANFILES = Makefile.in
