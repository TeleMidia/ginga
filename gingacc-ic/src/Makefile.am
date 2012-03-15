INCLUDES                = \
                          -I$(prefix)/include/ginga/ \
                          $(all_includes) \
                          -I$(top_srcdir)/include

LIBS                    =

libdir                  = \
                          $(prefix)/lib/ginga

lib_LTLIBRARIES         = \
                          libgingaccic.la

libgingaccic_la_SOURCES = \
                          InteractiveChannelManager.cpp

libgingaccic_la_LIBADD  = \
                          -lpthread \
                          $(CCRTP_LIBS) \
                          $(CURL_LIBS) \
                          -L$(top_srcdir)/src \
                          -L$(top_srcdir)/src/curlic \
                          -L$(top_srcdir)/src/ccrtpic

libgingaccic_la_LDFLAGS = \
                          -no-undefined \
                          -version-info 13:1:8

MAINTAINERCLEANFILES    = \
                          Makefile.in
