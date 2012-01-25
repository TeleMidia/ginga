
# set the include path found by configure
INCLUDES = \
          $(all_includes) \
          -I/usr/local/include/ginga/ \
          -I/usr/local/include/directfb/ \
	      -I$(top_srcdir)/include \
	      -I$(top_srcdir)/include/mb \
	      -I$(top_srcdir)/include/mb/interface \
	      $(KINECT_INCS)

LIBS =

MAINTAINERCLEANFILES = Makefile.in

#############
# gingaccmb #
#############
libgingaccmbdir = /usr/local/lib/ginga

libgingaccmb_so_LDFLAGS = -Wl,-soname,libgingaccmb.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../thread -L./

libgingaccmb_PROGRAMS = libgingaccmb.so

libgingaccmb_so_SOURCES = \
                          interface/CodeMap.cpp \
                          interface/InputEventFactory.cpp \
                          interface/Matrix.cpp \
                          InputManager.cpp \
                          LocalDeviceManager.cpp

libgingaccmb_so_LDADD   = \
                          -ltelemidiautil -lpthread \
                          -lgingaccsystem $(COMPONENTS_LIBS) $(KINECT_LIBS)
