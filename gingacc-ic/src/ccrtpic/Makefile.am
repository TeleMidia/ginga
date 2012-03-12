
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
$(all_includes) -I$(top_srcdir)/include

LIBS =

if CCRTPLIB
libdir = /usr/local/lib/ginga/ic

lib_LTLIBRARIES = libgingaccicccrtp.la

libgingaccicccrtp_la_SOURCES = CCRTPInteractiveChannel.cpp CCRTPClient.cpp

libgingaccicccrtp_la_LIBADD = -lccrtp1 $(COMPONENTS_LIBS)
endif
MAINTAINERCLEANFILES = Makefile.in
