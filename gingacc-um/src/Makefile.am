
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include \
-I$(prefix)/include/ginga/

LIBS =

libdir = $(prefix)/lib/ginga

libgingaccum_la_LIBADD = -ldl 

lib_LTLIBRARIES = libgingaccum.la
libgingaccum_la_SOURCES = Update.cpp

MAINTAINERCLEANFILES = Makefile.in
