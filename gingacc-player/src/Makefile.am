
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
                              -lgingaccsystem \
                              -lgingaccmb \
                              $(MULTIPROCESS_LIBS) \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib/ginga \
                              -L$(prefix)/lib

libgingaccplayer_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES        = \
                              Makefile.in
