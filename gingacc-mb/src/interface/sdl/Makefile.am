INCLUDES                   = \
                             -I$(prefix)/include/ginga/ \
                             -I$(prefix)/include/SDL2/ \
                             -I$(top_srcdir)/include \
                             $(all_includes)

LIBS                       =

libdir                     = \
                             $(prefix)/lib/ginga/mb

################
# gingaccmbsdl #
################
if SDLSYSTEM
lib_LTLIBRARIES            = \
                             libgingaccmbsdl.la

libgingaccmbsdl_la_SOURCES = \
                             SDLDeviceScreen.cpp \
                             input/SDLEventBuffer.cpp \
                             input/SDLInputEvent.cpp \
                             output/SDLSurface.cpp \
                             output/SDLWindow.cpp

libgingaccmbsdl_la_LIBADD  = \
                             -lSDL2 \
                             -ltelemidiautil \
                             -lpthread \
                             -lgingaccmb \
                             -L$(prefix)/lib \
                             -L$(prefix)/lib/ginga \
                             -L$(top_srcdir)/src
endif #SDLSYSTEM

MAINTAINERCLEANFILES       = \
                             Makefile.in
