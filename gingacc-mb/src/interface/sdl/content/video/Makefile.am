
# set the include path found by configure
INCLUDES                        = \
                                  -I$(prefix)/include/ginga/ \
                                  -I$(prefix)/include/SDL2/ \
                                  -I$(top_srcdir)/include \
                                  $(all_includes)

LIBS                            =

libdir                          = \
                                  $(prefix)/lib/ginga/mb/dec

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
                                  -L$(prefix)/lib \
                                  -L$(prefix)/lib/ginga \
                                  -L$(top_srcdir)/src \
                                  -L$(top_srcdir)/src/interface/sdl \
                                  -L$(top_srcdir)/src/interface/sdl/content/audio
endif #SDLSYSTEM

MAINTAINERCLEANFILES            = \
                                  Makefile.in
