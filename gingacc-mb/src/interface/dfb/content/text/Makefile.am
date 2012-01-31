
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

if DFBSYSTEM
localLibdir = /usr/local/lib/ginga/mb/dec/

libgingaccmbdecdfbf_so_LDFLAGS = -Wl,-soname,libgingaccmbdecdfbf.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga \
	-L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

localLib_PROGRAMS = libgingaccmbdecdfbf.so

libgingaccmbdecdfbf_so_SOURCES = DFBFontProvider.cpp

libgingaccmbdecdfbf_so_LDADD = $(DFB_LIBS) -lgingaccmbdfb \
    -lgingaccmb
endif

MAINTAINERCLEANFILES = Makefile.in
