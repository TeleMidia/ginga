INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include

LIBS                 =

noinst_PROGRAMS      = \
                       test

test_SOURCES         = \
                       test.cpp

test_LDADD           = \
                       -lgingaccsystem \
                       -ltelemidiautil \
                       -lgingacctuner

test_LDFLAGS         = \
                       -L../src/ \
                       -L$(prefix)/lib/ginga \
                       -L$(prefix)/lib

MAINTAINERCLEANFILES = \
                       Makefile.in
