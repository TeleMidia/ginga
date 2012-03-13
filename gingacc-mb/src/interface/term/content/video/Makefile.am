
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(FFMPEG_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/mb/dec

if TERMSYSTEM
lib_LTLIBRARIES = libgingaccmbdectermav.la

libgingaccmbdectermav_la_SOURCES = TermVideoProvider.cpp

libgingaccmbdectermav_la_LIBADD = -ltelemidiautil \
    -lgingaccmbterm -lgingaccmb -lgingaccsystem \
    -L$(prefix)/lib -L$(prefix)/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/term
endif #TERMSYSTEM

MAINTAINERCLEANFILES = Makefile.in
