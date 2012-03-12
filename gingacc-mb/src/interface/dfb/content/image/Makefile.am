
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/mb/dec

if DFBSYSTEM
lib_LTLIBRARIES = libgingaccmbdecdfbimg.la

libgingaccmbdecdfbimg_la_SOURCES = DFBImageProvider.cpp

libgingaccmbdecdfbimg_la_LIBADD = $(DFB_LIBS) -lgingaccmbdfb \
    -lgingaccmb -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb
endif

MAINTAINERCLEANFILES = Makefile.in
