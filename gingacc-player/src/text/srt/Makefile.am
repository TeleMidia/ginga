
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/players

lib_LTLIBRARIES = libgingaccplasrt.la

libgingaccplasrt_la_SOURCES = SrtPlayer.cpp

libgingaccplasrt_la_LIBADD = -ltelemidiautil -lgingaccsystem \
    -lgingaccplayer -lgingaccplatxt $(COMPONENTS_LIBS) \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../../ -L../

MAINTAINERCLEANFILES = Makefile.in
