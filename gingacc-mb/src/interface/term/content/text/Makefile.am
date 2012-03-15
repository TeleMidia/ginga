
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/mb/dec

if TERMSYSTEM
lib_LTLIBRARIES = libgingaccmbdectermf.la

libgingaccmbdectermf_la_SOURCES = TermFontProvider.cpp

libgingaccmbdectermf_la_LIBADD = -lgingaccmbterm -lgingaccmb \
-L$(prefix)/lib -L$(prefix)/lib/ginga \
	-L$(top_srcdir)/src -L$(top_srcdir)/src/interface/term

libgingaccmbdectermf_la_LDFLAGS = \
                                  -no-undefined \
                                  -version-info 13:1:8
endif #TERMSYSTEM

MAINTAINERCLEANFILES = Makefile.in
