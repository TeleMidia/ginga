
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

LIBS =
localLibdir = /usr/local/lib/ginga/iocontents/

libgingaccsysctsimgdfb_so_LDFLAGS = -Wl,-soname,libgingaccsysctsimgdfb.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L../../../

localLib_PROGRAMS = libgingaccsysctsimgdfb.so

libgingaccsysctsimgdfb_so_SOURCES = DFBImageProvider.cpp

libgingaccsysctsimgdfb_so_LDADD = $(DFB_LIBS) -lgingaccsystemiodfb \
    -lgingaccsystemio
