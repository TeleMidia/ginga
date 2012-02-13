
# set the include path found by configure
INCLUDES                        = \
                                  -I/usr/local/include/ginga/ \
                                  -I/usr/local/include/SDL2/ \
                                  -I$(top_srcdir)/include \
                                  $(all_includes)

LIBS                            =

if SDLSYSTEM
localLibsdldir                  = \
                                  /usr/local/lib/ginga/mb/dec/

localLibsdl_PROGRAMS            = \
                                  libgingaccmbdecsdlav.so

libgingaccmbdecsdlav_so_LDFLAGS = \
                                  -Wl,-soname,libgingaccmbdecsdlav.so \
                                  -shared \
                                  -L/usr/local/lib \
                                  -L/usr/local/lib/ginga \
                                  -L$(top_srcdir)/src \
                                  -L$(top_srcdir)/src/interface/sdl \
                                  -L$(top_srcdir)/src/interface/sdl/content/audio

libgingaccmbdecsdlav_so_SOURCES = \
                                  SDLVideoProvider.cpp

libgingaccmbdecsdlav_so_LDADD   = \
                                  -lSDL2 \
                                  -lSDL2_ffmpeg \
                                  -ltelemidiautil \
                                  -lgingaccmbsdl \
                                  -lgingaccmb \
                                  -lgingaccmbdecsdlap
endif #SDLSYSTEM

MAINTAINERCLEANFILES            = \
                                  Makefile.in
