# set the include path found by configure
INCLUDES                       = \
                                 -I/usr/local/include/ginga/ \
                                 -I/usr/local/include/SDL/ \
                                 -I$(top_srcdir)/include \
                                 $(all_includes)

LIBS =

if SDLSYSTEM
localLibdir                    = \
                                 /usr/local/lib/ginga/mb/dec/

libgingaccmbdecsdlf_so_LDFLAGS = \
                                 -Wl,-soname,libgingaccmbdecsdlf.so -shared \
                                 -L/usr/local/lib \
                                 -L/usr/local/lib/ginga \
                                 -L$(top_srcdir)/src \
                                 -L$(top_srcdir)/src/interface/sdl

localLib_PROGRAMS              = \
                                 libgingaccmbdecsdlf.so

libgingaccmbdecsdlf_so_SOURCES = \
                                 SDLFontProvider.cpp

libgingaccmbdecsdlf_so_LDADD   = \
                                 -lgingaccmbsdl \
                                 -lgingaccmb
endif

MAINTAINERCLEANFILES           = \
                                 Makefile.in
