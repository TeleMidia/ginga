
# set the include path found by configure
INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include

LIBS                 =

noinst_PROGRAMS      = \
                       test

test_SOURCES         = \
                       test.cpp

test_LDADD           = \
                       -lgingaccsystem

test_LDFLAGS         = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L$(prefix)/lib/ginga \
                       -L../src \
                       -L../src/component \
                       -L../src/parser

MAINTAINERCLEANFILES = \
                       Makefile.in
