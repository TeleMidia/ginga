INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include

LIBS                 =

noinst_PROGRAMS      = \
                       test

test_SOURCES         = \
                       test.cpp

test_LDADD           = \
                       $(TEST_COMPONENTS_LIBS)\
                       -ltelemidiautil \
                       -lgingaccsystem

test_LDFLAGS         = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L$(prefix)/lib/ginga \
                       -L../src

MAINTAINERCLEANFILES = \
                       Makefile.in
