INCLUDES                 = \
                           $(all_includes) \
                           -I/usr/local/include/ginga/ \
                           -I$(top_srcdir)/include \
                           -I$(top_srcdir)/include/mb \
                           -I$(top_srcdir)/include/mb/interface \
                           $(KINECT_INCS)

LIBS                     =

MAINTAINERCLEANFILES     = \
                           Makefile.in

#############
# gingaccmb #
#############
libdir                   = \
                           /usr/local/lib/ginga

lib_LTLIBRARIES          = \
                           libgingaccmb.la

libgingaccmb_la_SOURCES  = \
                           interface/CodeMap.cpp \
                           interface/InputEventFactory.cpp \
                           interface/Matrix.cpp \
                           InputManager.cpp \
                           LocalScreenManager.cpp

libgingaccmb_la_LIBADD   = \
                          -ltelemidiautil -lpthread \
                          -lgingaccsystem $(COMPONENTS_LIBS) $(KINECT_LIBS) \
                          -L/usr/local/lib -L/usr/local/lib/ginga \
                          -L./
