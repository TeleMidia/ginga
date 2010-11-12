
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

#####################
libgingaccsystemiocodemapdir = /usr/local/lib/ginga

libgingaccsystemiocodemap_so_LDFLAGS = -shared \
    -Wl,-soname,libgingaccsystemiocodemap.so \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../thread -L./

libgingaccsystemiocodemap_PROGRAMS = libgingaccsystemiocodemap.so

libgingaccsystemiocodemap_so_SOURCES =	interface/input/CodeMap.cpp

#####################
libgingaccsystemiodfbdir = /usr/local/lib/ginga

libgingaccsystemiodfb_so_LDFLAGS = -Wl,-soname,libgingaccsystemiodfb.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L../thread -L./

libgingaccsystemiodfb_PROGRAMS = libgingaccsystemiodfb.so

libgingaccsystemiodfb_so_SOURCES =	interface/device/dfb/DFBDeviceScreen.cpp \
	interface/input/dfb/DFBEventBuffer.cpp \
	interface/input/dfb/DFBGInputEvent.cpp \
	interface/output/dfb/DFBSurface.cpp interface/output/dfb/DFBWindow.cpp \
	interface/output/dfb/Matrix.cpp

libgingaccsystemiodfb_so_LDADD = $(DFB_LIBS) -ltelemidiautil -lpthread \
    -lgingaccsystemthread -lgingaccsystemio -lgingaccsystemiocodemap

######################
libgingaccsystemiodir = /usr/local/lib/ginga

libgingaccsystemio_so_LDFLAGS = -Wl,-soname,libgingaccsystemio.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../thread

libgingaccsystemio_PROGRAMS = libgingaccsystemio.so

libgingaccsystemio_so_SOURCES =	interface/device/DeviceAudio.cpp \
	interface/device/DeviceCommunication.cpp \
	interface/device/IODevice.cpp \
	GingaLocatorFactory.cpp InputManager.cpp LocalDeviceManager.cpp

libgingaccsystemio_so_LDADD = -ltelemidiautil -lpthread \
    -lgingaccsystemthread $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
