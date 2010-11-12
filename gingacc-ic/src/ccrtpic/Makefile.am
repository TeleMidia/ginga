
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes) -I$(top_srcdir)/include

LIBS =

if CCRTPLIB
libgingaccicccrtpdir = /usr/local/lib/ginga/ic

libgingaccicccrtp_so_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -Wl,-soname,libgingaccicccrtp.so -shared

libgingaccicccrtp_PROGRAMS = libgingaccicccrtp.so

libgingaccicccrtp_so_SOURCES = CCRTPInteractiveChannel.cpp CCRTPClient.cpp

libgingaccicccrtp_so_LDADD = -lccrtp1 $(COMPONENTS_LIBS)
endif
MAINTAINERCLEANFILES = Makefile.in
