
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/players

lib_LTLIBRARIES = libgingaccplaplt.la

libgingaccplaplt_la_SOURCES = PlainTxtPlayer.cpp

libgingaccplaplt_la_LIBADD = -ltelemidiautil -lgingaccplayer \
    -lpthread -lgingaccplatxt $(COMPONENTS_LIBS) \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../../ -L../

libgingaccplaplt_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
