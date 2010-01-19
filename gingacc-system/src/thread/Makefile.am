
# set the include path found by configure
INCLUDES = $(all_includes)
LIBS =
# the library search path.
localLibdir = /usr/local/lib/ginga

libgingaccsystemthread_so_LDFLAGS = -Wl,-soname,libgingaccsystemthread.so \
    -shared

localLib_PROGRAMS = libgingaccsystemthread.so
libgingaccsystemthread_so_SOURCES = Thread.cpp
libgingaccsystemthread_so_LDADD = -lpthread
