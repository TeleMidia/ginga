
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =
localLibdir = /usr/local/lib/ginga/mbdec/

libgingaccmbdecdfbimg_so_LDFLAGS = -Wl,-soname,libgingaccmbdecdfbimg.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

localLib_PROGRAMS = libgingaccmbdecdfbimg.so

libgingaccmbdecdfbimg_so_SOURCES = DFBImageProvider.cpp

libgingaccmbdecdfbimg_so_LDADD = $(DFB_LIBS) -lgingaccmbdfb \
    -lgingaccmb

MAINTAINERCLEANFILES = Makefile.in
