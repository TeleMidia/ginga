
# set the include path found by configure
INCLUDES                         = \
                                   -I/usr/local/include/ginga/ \
                                   -I/usr/local/include/SDL2/ \
                                   -I$(top_srcdir)/include \
                                   $(all_includes)

LIBS                             =

libdir                           = \
                                   /usr/local/lib/ginga/mb/dec

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
                                   -L/usr/local/lib \
                                   -L/usr/local/lib/ginga \
                                   -L$(top_srcdir)/src \
                                   -L$(top_srcdir)/src/interface/sdl
endif #SDLSYSTEM

MAINTAINERCLEANFILES             = \
                                   Makefile.in
