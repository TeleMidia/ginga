
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/players

lib_LTLIBRARIES = libgingaccplaplt.la

libgingaccplaplt_la_SOURCES = PlainTxtPlayer.cpp

libgingaccplaplt_la_LIBADD = -ltelemidiautil -lgingaccplayer \
    -lpthread -lgingaccplatxt $(COMPONENTS_LIBS) \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../../ -L../

MAINTAINERCLEANFILES = Makefile.in
