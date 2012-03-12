
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/players

lib_LTLIBRARIES = libgingaccplasrt.la

libgingaccplasrt_la_SOURCES = SrtPlayer.cpp

libgingaccplasrt_la_LIBADD = -ltelemidiautil -lgingaccsystem \
    -lgingaccplayer -lgingaccplatxt $(COMPONENTS_LIBS) \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../../ -L../

MAINTAINERCLEANFILES = Makefile.in
