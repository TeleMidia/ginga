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

libgingaccdataprocncledit_la_LDFLAGS = \
                                       -no-undefined \
                                       -version-info 13:1:8

MAINTAINERCLEANFILES                 = \
                                       Makefile.in
