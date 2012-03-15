
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ -I$(prefix)/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/mb/dec

if DFBSYSTEM
lib_LTLIBRARIES = libgingaccmbdecdfbimg.la

libgingaccmbdecdfbimg_la_SOURCES = DFBImageProvider.cpp

libgingaccmbdecdfbimg_la_LIBADD = $(DFB_LIBS) -lgingaccmbdfb \
    -lgingaccmb -L$(prefix)/lib -L$(prefix)/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbimg_la_LDFLAGS = \
                                   -no-undefined \
                                   -version-info 13:1:8

endif

MAINTAINERCLEANFILES = Makefile.in
