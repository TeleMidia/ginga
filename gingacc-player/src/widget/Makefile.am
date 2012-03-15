
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/players

lib_LTLIBRARIES = libgingaccplawdg.la

libgingaccplawdg_la_SOURCES = ShowButton.cpp

libgingaccplawdg_la_LIBADD = -lgingaccsystem -lgingaccplayer \
    -lgingaccplaimg $(COMPONENTS_LIBS) \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../ -L../image

libgingaccplawdg_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
