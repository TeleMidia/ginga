# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include

LIBS =

libgingacccmdir = /usr/local/lib/ginga

libgingacccm_so_LDFLAGS = -Wl,-soname,libgingacccm.so -shared -ldl
libgingacccm_PROGRAMS = libgingacccm.so
libgingacccm_so_SOURCES = ComponentManager.cpp
