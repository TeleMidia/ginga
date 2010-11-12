
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =

noinst_PROGRAMS = test

test_CPPFLAGS = -I$(top_srcdir)/include
test_SOURCES = test.cpp

test_LDADD = $(TEST_COMPONENTS_LIBS) -ltelemidiautil -L../src

MAINTAINERCLEANFILES = Makefile.in
