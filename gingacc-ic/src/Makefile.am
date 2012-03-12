
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga

lib_LTLIBRARIES = libgingaccic.la

libgingaccic_la_SOURCES = InteractiveChannelManager.cpp

libgingaccic_la_LIBADD = -lpthread $(CCRTP_LIBS) $(CURL_LIBS)

MAINTAINERCLEANFILES = Makefile.in
