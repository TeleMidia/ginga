
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

if TERMSYSTEM
localLibdir = /usr/local/lib/ginga/mb/dec/

libgingaccmbdectermimg_so_LDFLAGS = -Wl,-soname,libgingaccmbdectermimg.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/term

localLib_PROGRAMS = libgingaccmbdectermimg.so

libgingaccmbdectermimg_so_SOURCES = TermImageProvider.cpp

libgingaccmbdectermimg_so_LDADD = -lgingaccmbterm -lgingaccmb
endif

MAINTAINERCLEANFILES = Makefile.in
