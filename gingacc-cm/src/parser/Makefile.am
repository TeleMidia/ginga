
# set the include path found by configure
INCLUDES                      = \
                                $(all_includes) \
                                -I$(top_srcdir)/include

LIBS                          =

libdir                        = \
                                $(prefix)/lib/ginga/cm

lib_LTLIBRARIES               = \
                                libgingacccmparser.la

libgingacccmparser_la_LIBADD  = \
                                -lexpat \
                                -ldl

libgingacccmparser_la_SOURCES = \
                                ComponentParser.cpp

MAINTAINERCLEANFILES          = \
                                Makefile.in
