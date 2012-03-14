INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include \
                       -I$(prefix)/include/directfb

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
                       -lgingaccdataprocessing \
                       -lgingaccdataprocncl \
                       -lexpat \
                       -ldl

test_LDFLAGS         = \
                       -L$(prefix)/lib/ginga \
                       -L../src/ \
                       -L../src/dsmcc/npt \
                       -L../src/ncl -L$(prefix)/lib
endif

MAINTAINERCLEANFILES = \
                       Makefile.in
