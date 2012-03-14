INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include

LIBS                 =

if COMPONENTS
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
endif

MAINTAINERCLEANFILES = \
                       Makefile.in
