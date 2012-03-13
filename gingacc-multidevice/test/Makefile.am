
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =

noinst_PROGRAMS      = \
                       test

test_SOURCES = test.cpp

test_LDADD =  $(TEST_COMPONENTS_LIBS) -ltelemidiautil

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L$(prefix)/lib/ \
    -L$(prefix)/lib/ginga -L../src

MAINTAINERCLEANFILES = Makefile.in
