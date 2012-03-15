INCLUDES                = \
                          $(all_includes) \
                          -I$(top_srcdir)/include \
                          -I$(prefix)/include/ginga/

LIBS                    =

libdir                  = \
                          $(prefix)/lib/ginga

libgingaccum_la_LIBADD  = \
                          -lgingaccsystem

libgingaccum_la_LDFLAGS = \
                          -no-undefined \
                          -version-info 13:1:8

lib_LTLIBRARIES         = \
                          libgingaccum.la

libgingaccum_la_SOURCES = \
                          Update.cpp

MAINTAINERCLEANFILES    = \
                          Makefile.in
