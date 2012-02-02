
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include $(KINECT_INCS)

LIBS =

################
# gingaccmbdfb #
################
if DFBSYSTEM
libgingaccmbdfbdir         = \
                             /usr/local/lib/ginga/mb

libgingaccmbdfb_so_LDFLAGS = \
                             -Wl,-soname,libgingaccmbdfb.so -shared \
                             -L/usr/local/lib -L/usr/local/lib/ginga \
                             -L$(top_srcdir)/src

libgingaccmbdfb_PROGRAMS   = \
                             libgingaccmbdfb.so

libgingaccmbdfb_so_SOURCES = \
                             DFBDeviceScreen.cpp \
                             input/DFBEventBuffer.cpp \
                             input/DFBGInputEvent.cpp \
                             output/DFBSurface.cpp \
                             output/DFBWindow.cpp

libgingaccmbdfb_so_LDADD   = \
                             $(DFB_LIBS) -ltelemidiautil -lpthread \
                             -lgingaccsystem -lgingaccmb
endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
