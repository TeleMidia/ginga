
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

if TERMSYSTEM
localLibdir = /usr/local/lib/ginga/mb/dec/

libgingaccmbdectermap_so_LDFLAGS = -Wl,-soname,libgingaccmbdectermap.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga \
	-L$(top_srcdir)/src -L$(top_srcdir)/src/interface/term

localLib_PROGRAMS = libgingaccmbdectermap.so

libgingaccmbdectermap_so_SOURCES = TermAudioProvider.cpp

libgingaccmbdectermap_so_LDADD = -lgingaccmbterm -lgingaccmb
endif #TERMSYSTEM

MAINTAINERCLEANFILES = Makefile.in
