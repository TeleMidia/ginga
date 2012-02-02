
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

#################
# gingaccmbterm #
#################
if TERMSYSTEM
libgingaccmbtermdir         = \
                             /usr/local/lib/ginga/mb

libgingaccmbterm_so_LDFLAGS = \
                             -Wl,-soname,libgingaccmbterm.so -shared \
                             -L/usr/local/lib -L/usr/local/lib/ginga \
                             -L$(top_srcdir)/src

libgingaccmbterm_PROGRAMS   = \
                             libgingaccmbterm.so

libgingaccmbterm_so_SOURCES = \
                             TermDeviceScreen.cpp \
                             input/TermEventBuffer.cpp \
                             input/TermInputEvent.cpp \
                             output/TermSurface.cpp \
                             output/TermWindow.cpp

libgingaccmbterm_so_LDADD   = \
                             -ltelemidiautil -lpthread \
                             -lgingaccsystem -lgingaccmb
endif

MAINTAINERCLEANFILES = Makefile.in
