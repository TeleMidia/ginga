INCLUDES                         = \
                                   $(all_includes) \
                                   -I$(top_srcdir)/include

LIBS                             =

libdir                           = \
                                   $(prefix)/lib/ginga/dp

libgingaccdataprocncl_la_LIBADD  = \
                                   -lexpat \
                                   -ltelemidiautil \
                                   -lgingaccsystem \
                                   -L$(prefix)/lib/ginga

libgingaccdataprocncl_la_LDFLAGS = \
                                   -no-undefined \
                                   -version-info 13:1:8

lib_LTLIBRARIES                  = \
                                   libgingaccdataprocncl.la

libgingaccdataprocncl_la_SOURCES = \
                                   Metadata.cpp \
                                   DataFile.cpp \
                                   NCLSectionProcessor.cpp \
                                   parser/NCLMetadataParser.cpp

MAINTAINERCLEANFILES             = \
                                   Makefile.in
