
# set the include path found by configure
INCLUDES = $(all_includes)

LIBS =

libgingaccdataprocncldir = /usr/local/lib/ginga/dp

libgingaccdataprocncl_so_LDFLAGS = -Wl,-soname,libgingaccdataprocncl.so \
    -shared -lexpat -ltelemidiautil -L/usr/local/lib/ginga

libgingaccdataprocncl_PROGRAMS = libgingaccdataprocncl.so
libgingaccdataprocncl_so_SOURCES = Metadata.cpp DataFile.cpp \
    NCLSectionProcessor.cpp parser/NCLMetadataParser.cpp
