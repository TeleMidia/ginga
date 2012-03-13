
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga

lib_LTLIBRARIES = libgingaccic.la

libgingaccic_la_SOURCES = InteractiveChannelManager.cpp

libgingaccic_la_LIBADD = -lpthread $(CCRTP_LIBS) $(CURL_LIBS)

MAINTAINERCLEANFILES = Makefile.in
