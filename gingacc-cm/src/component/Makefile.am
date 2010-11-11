
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include

LIBS =

libgingacccmcomponentdir = /usr/local/lib/ginga/cm

libgingacccmcomponent_so_LDFLAGS = -Wl,-soname,libgingacccmcomponent.so \
    -shared

libgingacccmcomponent_PROGRAMS = libgingacccmcomponent.so
libgingacccmcomponent_so_SOURCES = Component.cpp
