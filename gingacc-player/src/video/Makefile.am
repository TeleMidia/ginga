
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/players

lib_LTLIBRARIES = libgingaccplaavp.la

CPPSRCS = AVPlayer.cpp

if ICRTP
CPPSRCS += RTPListener.cpp
endif

libgingaccplaavp_la_SOURCES = $(CPPSRCS)

libgingaccplaavp_la_LIBADD = -ltelemidiautil -lgingaccsystem \
    -lgingaccplayer $(COMPONENTS_LIBS) \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../

MAINTAINERCLEANFILES = Makefile.in
