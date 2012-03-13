
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =

noinst_PROGRAMS      = \
                       test

test_SOURCES = test.cpp

test_LDADD = -lgingaccsystem -ltelemidiautil -lgingacctuner \
    -lgingacctsparser -ldl

test_LDFLAGS = -L$(prefix)/lib/ginga -L../src/ -L$(prefix)/lib

MAINTAINERCLEANFILES = Makefile.in
