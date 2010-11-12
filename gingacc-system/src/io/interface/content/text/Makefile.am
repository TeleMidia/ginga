
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

localLibdir = /usr/local/lib/ginga/iocontents/

libgingaccsysctsdfbf_so_LDFLAGS = -Wl,-soname,libgingaccsysctsdfbf.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../../

localLib_PROGRAMS = libgingaccsysctsdfbf.so

libgingaccsysctsdfbf_so_SOURCES = DFBFontProvider.cpp

libgingaccsysctsdfbf_so_LDADD = $(DFB_LIBS) -lgingaccsystemiodfb \
    -lgingaccsystemio

MAINTAINERCLEANFILES = Makefile.in
