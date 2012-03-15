INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include \
                       -I$(prefix)/include/directfb

LIBS                 =

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
                       -lexpat

test_LDFLAGS         = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L$(prefix)/lib/ginga \
                       -L../src/ \
                       -L../src/dsmcc/npt \
                       -L../src/ncl -L$(prefix)/lib

MAINTAINERCLEANFILES = \
                       Makefile.in
