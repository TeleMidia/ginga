
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

#########
# IMAGE #
#########
libdir = /usr/local/lib/ginga/players

lib_LTLIBRARIES             = \
                              libgingaccplaimg.la

libgingaccplaimg_la_SOURCES = \
                              ImagePlayer.cpp

libgingaccplaimg_la_LIBADD  = \
                              -ltelemidiautil \
                              -lgingaccplayer \
                              $(COMPONENTS_LIBS) \
                              -L/usr/local/lib \
                              -L/usr/local/lib/ginga \
                              -L../

#########
# ANIME #
#########
lib_LTLIBRARIES += libgingaccplaani.la

libgingaccplaani_la_SOURCES = AnimePlayer.cpp

libgingaccplaani_la_LIBADD = -ltelemidiautil -lgingaccsystem \
    $(COMPONENTS_LIBS) -L/usr/local/lib -L/usr/local/lib/ginga -L../

MAINTAINERCLEANFILES = Makefile.in
