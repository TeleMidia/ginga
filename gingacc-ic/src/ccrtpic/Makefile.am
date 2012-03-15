
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
$(all_includes) -I$(top_srcdir)/include

LIBS =

if CCRTPLIB
libdir = $(prefix)/lib/ginga/ic

lib_LTLIBRARIES = libgingaccicccrtp.la

libgingaccicccrtp_la_SOURCES = CCRTPInteractiveChannel.cpp CCRTPClient.cpp

libgingaccicccrtp_la_LIBADD = -lccrtp1 $(COMPONENTS_LIBS)

libgingaccicccrtp_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

endif
MAINTAINERCLEANFILES = Makefile.in
