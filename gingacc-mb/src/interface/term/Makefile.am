INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

#################
# gingaccmbterm #
#################
libdir                      = \
                              /usr/local/lib/ginga/mb

if TERMSYSTEM
lib_LTLIBRARIES             = \
                              libgingaccmbterm.la

libgingaccmbterm_la_SOURCES = \
                              TermDeviceScreen.cpp \
                              input/TermEventBuffer.cpp \
                              input/TermInputEvent.cpp \
                              output/TermSurface.cpp \
                              output/TermWindow.cpp

libgingaccmbterm_la_LIBADD  = \
                              -ltelemidiautil -lpthread \
                              -lgingaccsystem -lgingaccmb \
                              -L/usr/local/lib -L/usr/local/lib/ginga \
                              -L$(top_srcdir)/src
endif #TERMSYSTEM

MAINTAINERCLEANFILES        = \
                              Makefile.in
