INCLUDES                             = \
                                       $(all_includes) \
                                       -I$(top_srcdir)/include

LIBS                                 =

libdir                               = \
                                       $(prefix)/lib/ginga/dp

lib_LTLIBRARIES                      = \
                                       libgingaccdataprocncledit.la

libgingaccdataprocncledit_la_LIBADD  = \
                                       -ltelemidiautil \
                                       -L$(prefix)/lib/ginga

libgingaccdataprocncledit_la_SOURCES = \
                                       EventDescriptor.cpp

MAINTAINERCLEANFILES                 = \
                                       Makefile.in
