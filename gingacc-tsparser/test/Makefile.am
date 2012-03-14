INCLUDES             = \
                       $(all_includes) -I$(top_srcdir)/include

LIBS                 =

if COMPONENTS
noinst_PROGRAMS      = \
                       test

test_SOURCES         = \
                       test.cpp

test_LDADD           = \
                       -lgingaccsystem \
                       -ltelemidiautil \
                       -lgingacctuner \
                       -lgingacctsparser \
                       -ldl

test_LDFLAGS         = \
                       -L$(prefix)/lib/ginga \
                       -L../src/ \
                       -L$(prefix)/lib
endif

MAINTAINERCLEANFILES = \
                       Makefile.in
