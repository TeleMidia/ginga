
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD = -lgingaccsystemthread -ltelemidiautil -lgingacctuner \
    -lgingacctsparser -lgingaccdataprocessing \
    -lgingaccdataprocncl -ldl

test_LDFLAGS = -L/usr/local/lib/ginga -L../src/ -L../src/dsmcc/npt \
    -L../src/ncl -L/usr/local/lib

MAINTAINERCLEANFILES = Makefile.in
