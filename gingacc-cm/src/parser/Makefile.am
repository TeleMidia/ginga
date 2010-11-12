
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include

LIBS =

libgingacccmparserdir = /usr/local/lib/ginga/cm

libgingacccmparser_so_LDFLAGS = -Wl,-soname,libgingacccmparser.so \
    -shared -ldl -lexpat

libgingacccmparser_PROGRAMS = libgingacccmparser.so
libgingacccmparser_so_SOURCES = ComponentParser.cpp

MAINTAINERCLEANFILES = Makefile.in
