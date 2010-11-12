
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include

LIBS =

libgingaccumdir = /usr/local/lib/ginga

libgingaccum_so_LDFLAGS = -Wl,-soname,libgingaccum.so \
    -shared -ldl -I/usr/local/include/ginga/

libgingaccum_PROGRAMS = libgingaccum.so
libgingaccum_so_SOURCES = Update.cpp

MAINTAINERCLEANFILES = Makefile.in
