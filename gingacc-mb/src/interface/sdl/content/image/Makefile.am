
# set the include path found by configure
INCLUDES                         = \
                                   -I/usr/local/include/ginga/ \
                                   -I/usr/local/include/SDL2/ \
                                   -I$(top_srcdir)/include \
                                   $(all_includes)

LIBS                             =

if SDLSYSTEM
localLibdir                      = \
                                   /usr/local/lib/ginga/mb/dec/

libgingaccmbdecsdlimg_so_LDFLAGS = \
                                   -Wl,-soname,libgingaccmbdecsdlimg.so \
                                   -shared \
                                   -L/usr/local/lib \
                                   -L/usr/local/lib/ginga \
                                   -L$(top_srcdir)/src \
                                   -L$(top_srcdir)/src/interface/sdl

localLib_PROGRAMS                = \
                                   libgingaccmbdecsdlimg.so

libgingaccmbdecsdlimg_so_SOURCES = \
                                   SDLImageProvider.cpp

libgingaccmbdecsdlimg_so_LDADD   = \
                                   -lSDL2 \
                                   -lSDL2_image \
                                   -lgingaccmbsdl \
                                   -lgingaccmb
endif #SDLSYSTEM

MAINTAINERCLEANFILES             = \
                                   Makefile.in
