INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

#################
# gingaccmbterm #
#################
libdir                      = \
                              $(prefix)/lib/ginga/mb

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
                              -L$(prefix)/lib -L$(prefix)/lib/ginga \
                              -L$(top_srcdir)/src

libgingaccmbterm_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8
endif #TERMSYSTEM

MAINTAINERCLEANFILES        = \
                              Makefile.in
