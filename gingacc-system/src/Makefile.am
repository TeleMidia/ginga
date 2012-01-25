
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =
# the library search path.
localLibdir = /usr/local/lib/ginga

SYSTEMSOURCES = thread/Thread.cpp fs/GingaLocatorFactory.cpp

if MULTIPROCESS
SYSTEMSOURCES =+ process/Process.cpp process/SpawnedProcess.cpp
endif

libgingaccsystem_so_LDFLAGS = -Wl,-soname,libgingaccsystem.so -shared
localLib_PROGRAMS           = libgingaccsystem.so
libgingaccsystem_so_SOURCES = $(SYSTEMSOURCES)
libgingaccsystem_so_LDADD   = -lpthread $(MULTIPROCESS_LIBS) $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
