INCLUDES                             = \
                                       -I/usr/local/include/ginga/ \
                                       -I/usr/include/libuser/ \
                                       -I../include \
                                       $(all_includes) \
                                       -I$(top_srcdir)/include/contextmanager

LIBS                                 =

libdir                               = \
                                       /usr/local/lib/ginga

lib_LTLIBRARIES                      = \
                                       libgingacccontextmanager.la

libgingacccontextmanager_la_SOURCES  = \
                                       ContextManager.cpp \
                                       user/GingaUser.cpp \
                                       system/SystemInfo.cpp

libgingacccontextmanager_la_LIBADD   = \
                                       -ltelemidiautil \
                                       $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
