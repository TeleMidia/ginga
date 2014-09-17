include $(top_srcdir)/build-aux/Makefile.am.common

############################## gingacc-system ##############################
AM_CPPFLAGS =\
  -DSTR_PREFIX='"$(prefix)"'\
  -DGINGA_LIBDIR='"$(gingalibdir)"'\
  -DGINGA_INI_PATH='"$(gingaconfdir)/ginga.ini"'\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LIBS) $(DL_LIBS)

gingaccsystemlib_LTLIBRARIES = libgingaccsystem.la

libgingaccsystem_la_SOURCES =\
  src/thread/Thread.cpp\
  src/fs/GingaLocatorFactory.cpp\
  src/compat/PracticalSocket.cpp\
  src/compat/SystemCompat.cpp\
  src/time/Stc.cpp\
  $(NULL)

if BUILD_MULTIDEVICE
AM_CXXFLAGS += $(LIBZIP_CFLAGS)
AM_LDFLAGS += $(LIBZIP_LIBS)
endif

if BUILD_MULTIPROCESS
AM_LDFLAGS += $(RT_LIBS)
libgingaccsystem_la_SOURCES +=\
  src/process/Process.cpp\
  src/process/SpawnedProcess.cpp\
  $(NULL)
endif

################################# Headers ##################################
if INSTALL_HEADERS
systemincludedir = $(gingaccsystemincludedir)
systeminclude_HEADERS =\
  include/system/IComponentInstance.h\
  $(NULL)

compatincludedir = $(gingaccsystemincludedir)/compat
compatinclude_HEADERS =\
  include/system/compat/PracticalSocket.h\
  include/system/compat/SystemCompat.h\
  $(NULL)

fsincludedir = $(gingaccsystemincludedir)/fs
fsinclude_HEADERS =\
  include/system/fs/IGingaLocatorFactory.h\
  include/system/fs/GingaLocatorFactory.h\
  $(NULL)

if BUILD_MULTIPROCESS
processincludedir = $(gingaccsystemincludedir)/process
processinclude_HEADERS =\
  include/system/process/IProcessListener.h\
  include/system/process/Process.h\
  include/system/process/SpawnedProcess.h\
  $(NULL)
endif

threadincludedir = $(gingaccsystemincludedir)/thread
threadinclude_HEADERS =\
  include/system/thread/Thread.h\
  $(NULL)

timeincludedir = $(gingaccsystemincludedir)/time
timeinclude_HEADERS =\
  include/system/time/ITimeBaseListener.h\
  include/system/time/ITimeBaseProvider.h\
  include/system/time/Stc.h\
  $(NULL)
endif
