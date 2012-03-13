
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
$(all_includes) -I$(top_srcdir)/include

LIBS =

if CURLLIB
libdir = $(prefix)/lib/ginga/ic

lib_LTLIBRARIES = libgingacciccurlic.la

libgingacciccurlic_la_SOURCES = CurlInteractiveChannel.cpp

libgingacciccurlic_la_LIBADD = -lcurl -ltelemidiautil -lpthread \
    $(COMPONENTS_LIBS)

endif
MAINTAINERCLEANFILES = Makefile.in
