include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS)

gingacccontextmanagerlib_LTLIBRARIES = libgingacccontextmanager.la

libgingacccontextmanager_la_SOURCES =\
  src/ContextManager.cpp\
  src/user/GingaUser.cpp\
  src/system/SystemInfo.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
gingacccontextmanagerinclude_HEADERS =\
  include/contextmanager/IContextManager.h\
  include/contextmanager/IContextListener.h\
  include/contextmanager/user/IGingaUser.h\
  include/contextmanager/system/ISystemInfo.h\
  $(NULL)

systemincludedir = $(gingacccontextmanagerincludedir)/system
systeminclude_HEADERS =\
  include/contextmanager/system/ISystemInfo.h\
  $(NULL)

userincludedir = $(gingacccontextmanagerincludedir)/user
userinclude_HEADERS =\
  include/contextmanager/user/IGingaUser.h\
  $(NULL)
endif
############################## Configuration ###############################
gingacccontextmanagerconfdir = $(gingaconfdir)/contextmanager
gingacccontextmanagerconf_DATA =\
  files/users.ini\
  files/contexts.ini\
  $(NULL)
