
# set the include path found by configure
INCLUDES                    = \
                              -I$(prefix)/include/ginga/ \
                              $(all_includes) \
                              -I$(top_srcdir)/include

LIBS                        =

libdir                      = \
                              $(prefix)/lib/ginga/players

lib_LTLIBRARIES             = \
                              libgingaccplatxt.la

libgingaccplatxt_la_SOURCES = \
                              TextPlayer.cpp

libgingaccplatxt_la_LIBADD  = \
                              -ltelemidiautil \
                              -lgingaccsystem \
                              -lgingaccplayer \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib \
                              -L$(prefix)/lib/ginga \
                              -L../

libgingaccplatxt_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES        = \
                              Makefile.in
