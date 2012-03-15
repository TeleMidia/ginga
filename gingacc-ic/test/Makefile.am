INCLUDES             = \
                       $(all_includes) \
                       -I$(top_srcdir)/include

LIBS                 =

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
                       -ltelemidiautil \
                       -lgingaccsystem

test_LDFLAGS         = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L../src \
                       -L../src/curlic \
                       -L../src/ccrtpic

MAINTAINERCLEANFILES = \
                       Makefile.in
