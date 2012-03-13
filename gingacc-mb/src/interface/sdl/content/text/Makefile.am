# set the include path found by configure
INCLUDES                       = \
                                 -I$(prefix)/include/ginga/ \
                                 -I$(prefix)/include/SDL2/ \
                                 -I$(top_srcdir)/include \
                                 $(all_includes)

LIBS =

libdir                         = \
                                 $(prefix)/lib/ginga/mb/dec

if SDLSYSTEM
lib_LTLIBRARIES                = \
                                 libgingaccmbdecsdlf.la

libgingaccmbdecsdlf_la_SOURCES = \
                                 SDLFontProvider.cpp

libgingaccmbdecsdlf_la_LIBADD  = \
                                 -lSDL2 \
                                 -lSDL2_ttf \
                                 -lgingaccmbsdl \
                                 -lgingaccmb \
                                 -lgingaccsystem \
                                 -L$(prefix)/lib \
                                 -L$(prefix)/lib/ginga \
                                 -L$(top_srcdir)/src \
                                 -L$(top_srcdir)/src/interface/sdl
endif #SDLSYSTEM

MAINTAINERCLEANFILES           = \
                                 Makefile.in
