
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/mb/dec

if TERMSYSTEM
lib_LTLIBRARIES = libgingaccmbdectermimg.la

libgingaccmbdectermimg_la_SOURCES = TermImageProvider.cpp

libgingaccmbdectermimg_la_LIBADD = -lgingaccmbterm -lgingaccmb \
-L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/term
endif #TERMSYSTEM

MAINTAINERCLEANFILES = Makefile.in
