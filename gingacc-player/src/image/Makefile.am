INCLUDES                    = \
                              -I$(prefix)/include/ginga/ \
                              $(all_includes) \
                              -I$(top_srcdir)/include

LIBS                        =

#########
# IMAGE #
#########
libdir                      = \
                              $(prefix)/lib/ginga/players

lib_LTLIBRARIES             = \
                              libgingaccplaimg.la

libgingaccplaimg_la_SOURCES = \
                              ImagePlayer.cpp

libgingaccplaimg_la_LIBADD  = \
                              -ltelemidiautil \
                              -lgingaccsystem \
                              -lgingaccplayer \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib \
                              -L$(prefix)/lib/ginga \
                              -L../

#########
# ANIME #
#########
lib_LTLIBRARIES            += \
                              libgingaccplaani.la

libgingaccplaani_la_SOURCES = \
                              AnimePlayer.cpp

libgingaccplaani_la_LIBADD  = \
                              -ltelemidiautil \
                              -lgingaccsystem \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib \
                              -L$(prefix)/lib/ginga \
                              -L../

MAINTAINERCLEANFILES        = \
                              Makefile.in
