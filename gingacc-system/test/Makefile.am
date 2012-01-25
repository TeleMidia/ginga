
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
    $(FFMPEG_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD = -ltelemidiautil -lgingaccsystem

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L../src/ -L/usr/local/lib/ginga 

MAINTAINERCLEANFILES = Makefile.in
