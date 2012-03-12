
# set the include path found by configure
INCLUDES                        = \
                                  -I/usr/local/include/ginga/ \
                                  -I/usr/local/include/SDL2/ \
                                  -I$(top_srcdir)/include \
                                  $(all_includes)

LIBS                            =

libdir                          = \
                                  /usr/local/lib/ginga/mb/dec

if SDLSYSTEM
lib_LTLIBRARIES                 = \
                                  libgingaccmbdecsdlav.la

libgingaccmbdecsdlav_la_SOURCES = \
                                  SDLVideoProvider.cpp

libgingaccmbdecsdlav_la_LIBADD   = \
                                  -ltelemidiautil \
                                  -lgingaccsystem \
                                  -lgingaccmb \
                                  -lgingaccmbsdl \
                                  -lgingaccmbdecsdlap \
                                  -lSDL2 \
                                  -lpthread \
                                  -lavcodec \
                                  -lavformat \
                                  -lswresample \
                                  -lswscale \
                                  -L/usr/local/lib \
                                  -L/usr/local/lib/ginga \
                                  -L$(top_srcdir)/src \
                                  -L$(top_srcdir)/src/interface/sdl \
                                  -L$(top_srcdir)/src/interface/sdl/content/audio
endif #SDLSYSTEM

MAINTAINERCLEANFILES            = \
                                  Makefile.in
