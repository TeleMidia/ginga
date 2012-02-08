
# set the include path found by configure
INCLUDES                   = \
                             -I/usr/local/include/ginga/ \
                             -I/usr/local/include/SDL2/ \
                             -I$(top_srcdir)/include \
                             $(all_includes)

LIBS                       =

################
# gingaccmbsdl #
################
if SDLSYSTEM
libgingaccmbsdldir         = \
                             /usr/local/lib/ginga/mb

libgingaccmbsdl_so_LDFLAGS = \
                             -Wl,-soname,libgingaccmbsdl.so -shared \
                             -L/usr/local/lib -L/usr/local/lib/ginga \
                             -L$(top_srcdir)/src

libgingaccmbsdl_PROGRAMS   = \
                             libgingaccmbsdl.so

libgingaccmbsdl_so_SOURCES = \
                             SDLDeviceScreen.cpp \
                             input/SDLEventBuffer.cpp \
                             input/SDLInputEvent.cpp \
                             output/SDLSurface.cpp \
                             output/SDLWindow.cpp

libgingaccmbsdl_so_LDADD   = \
                             -lSDL2 -ltelemidiautil -lpthread \
                             -lgingaccsystem -lgingaccmb
endif #SDLSYSTEM

MAINTAINERCLEANFILES       = \
                             Makefile.in
