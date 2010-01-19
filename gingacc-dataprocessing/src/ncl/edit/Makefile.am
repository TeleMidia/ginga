
# set the include path found by configure
INCLUDES = $(all_includes)

LIBS =

libgingaccdataprocncleditdir = /usr/local/lib/ginga/dp

libgingaccdataprocncledit_so_LDFLAGS = -Wl,-soname,libgingaccdataprocncledit.so \
    -shared -ltelemidiautil -L/usr/local/lib/ginga

libgingaccdataprocncledit_PROGRAMS = libgingaccdataprocncledit.so
libgingaccdataprocncledit_so_SOURCES = EventDescriptor.cpp
