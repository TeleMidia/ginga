INCLUDES                    = \
                              -I$(prefix)/include/ginga/ \
                              $(all_includes) \
                              -I$(top_srcdir)/include \
                              $(GTK_INCS)

LIBS                        =

libdir                      = \
                              $(prefix)/lib/ginga/players

if SMILLIB
lib_LTLIBRARIES             = \
                              libgingaccplasml.la

libgingaccplasml_la_SOURCES = \
                              SmilPlayer.cpp

# TODO: figure how to clean this
libgingaccplasml_la_LIBADD  = \
                              gtk_mainloop.o \
                              -lambulant \
                              -lambulant_gtk \
                              -lambulant_sdl \
                              -ltelemidiautil \
                              -lgingaccplayer \
                              -lgingaccsystem \
                              -lpthread \
                              $(GTK_LIBS) \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib \
                              -L$(prefix)/lib/ginga \
                              -L../../../../

libgingaccplasml_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8
endif
MAINTAINERCLEANFILES        = \
                              Makefile.in
