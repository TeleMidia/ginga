
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =

noinst_PROGRAMS      = \
                       test

test_SOURCES = test.cpp

test_LDADD = -lgingaccsystem -ltelemidiautil -lgingacctuner

test_LDFLAGS = -L../src/ -L/usr/local/lib/ginga -L/usr/local/lib

MAINTAINERCLEANFILES = Makefile.in
