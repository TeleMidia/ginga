INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include

LIBS                 =

if COMPONENTS
noinst_PROGRAMS      = \
                       test

test_SOURCES         = \
                       test.cpp

test_CFLAGS          = \
                       -I$(top_srcdir)/include

test_LDADD           = \
                       $(CCRTP_LIBS) \
                       $(CURL_LIBS) \
                       $(TEST_COMPONENTS_LIBS) \
                       -ltelemidiautil

test_LDFLAGS         = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L../src \
                       -L../src/curlic \
                       -L../src/ccrtpic
endif
MAINTAINERCLEANFILES = \
                       Makefile.in
