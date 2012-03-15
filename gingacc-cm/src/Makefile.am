INCLUDES                = \
                          $(all_includes) \
                          -I$(top_srcdir)/include \
                          -I$(prefix)/include/ginga

LIBS                    =

libdir                  = \
                          $(prefix)/lib/ginga

lib_LTLIBRARIES         = \
                          libgingacccm.la
                          
libgingacccm_la_LIBADD  = \
                          -lgingaccsystem \
                          -L$(prefix)/lib/ginga

libgingacccm_la_LDFLAGS = \
                          -no-undefined \
                          -version-info 13:1:8

libgingacccm_la_SOURCES = \
                          ComponentManager.cpp

MAINTAINERCLEANFILES    = \
                          Makefile.in
