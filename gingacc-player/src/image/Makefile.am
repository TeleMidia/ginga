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

libgingaccplaimg_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES        = \
                              Makefile.in
