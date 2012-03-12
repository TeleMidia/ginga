
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include
LIBS =
# the library search path.
libdir = /usr/local/lib/ginga

SYSTEMSOURCES = thread/Thread.cpp fs/GingaLocatorFactory.cpp

if MULTIPROCESS
SYSTEMSOURCES =+ process/Process.cpp process/SpawnedProcess.cpp
endif

lib_LTLIBRARIES              = libgingaccsystem.la
libgingaccsystem_la_SOURCES  = $(SYSTEMSOURCES)
libgingaccsystem_la_LIBADD   = -lpthread $(MULTIPROCESS_LIBS) $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
