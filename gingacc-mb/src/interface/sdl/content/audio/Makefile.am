
# set the include path found by configure
INCLUDES                        = \
                                  -I/usr/local/include/ginga/ \
                                  -I/usr/local/include/SDL2/ \
                                  -I$(top_srcdir)/include \
                                  $(all_includes)

LIBS                            =

if SDLSYSTEM
localLibdir                     = \
                                  /usr/local/lib/ginga/mb/dec/

libgingaccmbdecsdlap_so_LDFLAGS = \
                                  -Wl,-soname,libgingaccmbdecsdlap.so -shared \
	                              -L/usr/local/lib \
	                              -L/usr/local/lib/ginga \
	                              -L$(top_srcdir)/src \
	                              -L$(top_srcdir)/src/interface/sdl

localLib_PROGRAMS               = \
                                  libgingaccmbdecsdlap.so

libgingaccmbdecsdlap_so_SOURCES = \
                                  SDLAudioProvider.cpp

libgingaccmbdecsdlap_so_LDADD   = \
                                  -lgingaccmbsdl \
                                  -lgingaccmb \
                                  -lSDL2 -lavformat -lswscale -lSDL2_ffmpeg
endif #SDLSYSTEM

MAINTAINERCLEANFILES            = \
                                  Makefile.in
