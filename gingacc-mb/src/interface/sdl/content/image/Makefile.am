
# set the include path found by configure
INCLUDES                         = \
                                   -I$(prefix)/include/ginga/ \
                                   -I$(prefix)/include/SDL2/ \
                                   -I$(top_srcdir)/include \
                                   $(all_includes)

LIBS                             =

libdir                           = \
                                   $(prefix)/lib/ginga/mb/dec

if SDLSYSTEM
lib_LTLIBRARIES                  = \
                                   libgingaccmbdecsdlimg.la

libgingaccmbdecsdlimg_la_SOURCES = \
                                   SDLImageProvider.cpp

libgingaccmbdecsdlimg_la_LIBADD  = \
                                   -lSDL2 \
                                   -lSDL2_image \
                                   -lgingaccmbsdl \
                                   -lgingaccmb \
                                   -L$(prefix)/lib \
                                   -L$(prefix)/lib/ginga \
                                   -L$(top_srcdir)/src \
                                   -L$(top_srcdir)/src/interface/sdl

libgingaccmbdecsdlimg_la_LDFLAGS = \
                                   -no-undefined \
                                   -version-info 13:1:8

endif #SDLSYSTEM

MAINTAINERCLEANFILES             = \
                                   Makefile.in
