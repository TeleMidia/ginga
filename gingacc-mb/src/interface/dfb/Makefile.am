
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ -I$(prefix)/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include $(KINECT_INCS)

LIBS =

libdir                     = \
                             $(prefix)/lib/ginga/mb

################
# gingaccmbdfb #
################
if DFBSYSTEM

lib_LTLIBRARIES            = \
                             libgingaccmbdfb.la

libgingaccmbdfb_la_SOURCES = \
                             DFBDeviceScreen.cpp \
                             input/DFBEventBuffer.cpp \
                             input/DFBGInputEvent.cpp \
                             output/DFBSurface.cpp \
                             output/DFBWindow.cpp

libgingaccmbdfb_la_LIBADD   = \
                             $(DFB_LIBS) -ltelemidiautil -lpthread \
                             -lgingaccsystem -lgingaccmb \
                             -L$(prefix)/lib -L$(prefix)/lib/ginga \
                             -L$(top_srcdir)/src
                             
endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
