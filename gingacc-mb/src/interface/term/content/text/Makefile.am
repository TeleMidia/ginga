
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

if TERMSYSTEM
localLibdir = /usr/local/lib/ginga/mb/dec/

libgingaccmbdectermf_so_LDFLAGS = -Wl,-soname,libgingaccmbdectermf.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga \
	-L$(top_srcdir)/src -L$(top_srcdir)/src/interface/term

localLib_PROGRAMS = libgingaccmbdectermf.so

libgingaccmbdectermf_so_SOURCES = TermFontProvider.cpp

libgingaccmbdectermf_so_LDADD = -lgingaccmbterm -lgingaccmb
endif #TERMSYSTEM

MAINTAINERCLEANFILES = Makefile.in
