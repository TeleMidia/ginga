
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(FFMPEG_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

if TERMSYSTEM
localLibtermdir = /usr/local/lib/ginga/mb/dec/

localLibterm_PROGRAMS = libgingaccmbdectermav.so

libgingaccmbdectermav_so_LDFLAGS = -Wl,-soname,libgingaccmbdectermav.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/term

libgingaccmbdectermav_so_SOURCES = TermVideoProvider.cpp

libgingaccmbdectermav_so_LDADD = -ltelemidiautil \
    -lgingaccmbterm -lgingaccmb -lgingaccsystem
endif #TERMSYSTEM

MAINTAINERCLEANFILES = Makefile.in
