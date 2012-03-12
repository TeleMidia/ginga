
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/dp

libgingaccdataprocncl_la_LIBADD = -lexpat -ltelemidiautil -L/usr/local/lib/ginga

lib_LTLIBRARIES = libgingaccdataprocncl.la
libgingaccdataprocncl_la_SOURCES = Metadata.cpp DataFile.cpp \
    NCLSectionProcessor.cpp parser/NCLMetadataParser.cpp

MAINTAINERCLEANFILES = Makefile.in
