INCLUDES             = \
                       -I$(prefix)/include/ginga/ \
                       $(all_includes) \
                       -I$(top_srcdir)/include

LIBS                 =

noinst_PROGRAMS      = \
                       test

test_SOURCES         = \
                       test.cpp

test_LDADD           = \
                       -ltelemidiautil \
                       -lgingaccsystem

test_LDFLAGS         = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L../src/ \
                       -L$(prefix)/lib/ginga 

MAINTAINERCLEANFILES = \
                       Makefile.in
