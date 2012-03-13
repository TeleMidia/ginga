
# set the include path found by configure
INCLUDES                    = \
                              -I$(prefix)/include/ginga/ \
                              $(all_includes) \
                              -I$(top_srcdir)/include

LIBS                        =

libdir                      = \
                              $(prefix)/lib/ginga

lib_LTLIBRARIES             = \
                              libgingaccplayer.la

CPPSRCS                     = \
                              Player.cpp

if MULTIPROCESS
CPPSRCS                    += \
                              PlayerProcess.cpp
endif

libgingaccplayer_la_SOURCES = \
                              $(CPPSRCS)

libgingaccplayer_la_LIBADD  = \
                              -lpthread \
                              -ltelemidiautil \
                              -lgingaccmb \
                              $(MULTIPROCESS_LIBS) \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib/ginga \
                              -L$(prefix)/lib

MAINTAINERCLEANFILES        = \
                              Makefile.in
