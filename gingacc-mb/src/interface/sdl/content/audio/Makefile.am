
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
                                  libgingaccmbdecsdlap.la

libgingaccmbdecsdlap_la_SOURCES = \
                                  SDLAudioProvider.cpp \
                                  SDL2ffmpeg.cpp

libgingaccmbdecsdlap_la_LIBADD   = \
                                  -ltelemidiautil \
                                  -lgingaccsystem \
                                  -lgingaccmb \
                                  -lgingaccmbsdl \
                                  -lSDL2 \
                                  -lpthread \
                                  -lavcodec \
                                  -lavformat \
                                  -lswresample \
                                  -lswscale \
                                  -L$(prefix)/lib \
	                              -L$(prefix)/lib/ginga \
	                              -L$(top_srcdir)/src \
	                              -L$(top_srcdir)/src/interface/sdl

libgingaccmbdecsdlap_la_LDFLAGS = \
                                  -no-undefined \
                                  -version-info 13:1:8

endif #SDLSYSTEM

MAINTAINERCLEANFILES            = \
                                  Makefile.in
