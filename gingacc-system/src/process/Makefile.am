
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ $(all_includes)

LIBS =
# the library search path.
localLibdir = /usr/local/lib/ginga

libgingaccsystemprocess_so_LDFLAGS = -Wl,-soname,libgingaccsystemprocess.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga 

localLib_PROGRAMS = libgingaccsystemprocess.so
libgingaccsystemprocess_so_SOURCES = Process.cpp SpawnedProcess.cpp
libgingaccsystemprocess_so_LDADD = -ltelemidiautil -lpthread -lrt \
    $(COMPONENTS_LIBS)
