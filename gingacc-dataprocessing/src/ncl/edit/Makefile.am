INCLUDES                             = \
                                       $(all_includes) \
                                       -I$(top_srcdir)/include

LIBS                                 =

libdir                               = \
                                       /usr/local/lib/ginga/dp

lib_LTLIBRARIES                      = \
                                       libgingaccdataprocncledit.la

libgingaccdataprocncledit_la_LIBADD  = \
                                       -ltelemidiautil \
                                       -L/usr/local/lib/ginga

libgingaccdataprocncledit_la_SOURCES = \
                                       EventDescriptor.cpp

MAINTAINERCLEANFILES                 = \
                                       Makefile.in
