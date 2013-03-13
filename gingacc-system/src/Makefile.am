INCLUDES                     = \
                               $(all_includes) \
                               -I$(top_srcdir)/include

LIBS                         =

libdir                       = \
                               $(prefix)/lib/ginga

SYSTEMSOURCES                = \
                               thread/Thread.cpp \
                               fs/GingaLocatorFactory.cpp \
                               compat/PracticalSocket.cpp \
                               compat/SystemCompat.cpp \
                               time/Stc.cpp

if MULTIPROCESS
SYSTEMSOURCES               += \
                               process/Process.cpp \
                               process/SpawnedProcess.cpp
endif

lib_LTLIBRARIES              = \
                               libgingaccsystem.la

libgingaccsystem_la_SOURCES  = \
                               $(SYSTEMSOURCES)

libgingaccsystem_la_LIBADD   = \
                               -lpthread \
                               -ltelemidiautil \
                               $(MULTIPROCESS_LIBS) \
                               $(COMPONENTS_LIBS) \
                               $(DL_LIBS)

libgingaccsystem_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES         = \
                               Makefile.in
