
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
    $(all_includes) -I$(top_srcdir)/include

LIBS =

libgingaccmddir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libgingaccmd_so_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -Wl,-soname,libgingaccmd.so -shared

libgingaccmd_PROGRAMS = libgingaccmd.so

libgingaccmd_so_SOURCES = RemoteDeviceManager.cpp RemoteDevice.cpp \
	services/RemoteEventService.cpp \
    services/DeviceDomain.cpp \
    services/BaseDeviceDomain.cpp \
    services/ActiveDeviceDomain.cpp \
    services/PassiveDeviceDomain.cpp \
    services/device/DeviceService.cpp \
    services/device/BaseDeviceService.cpp \
    services/device/ActiveDeviceService.cpp \
    services/device/PassiveDeviceService.cpp \
    services/network/NetworkUtil.cpp \
    services/network/BroadcastSocketService.cpp \
    services/network/MulticastSocketService.cpp \
    services/network/TcpSocketService.cpp \
    services/network/TcpClientConnection.cpp

libgingaccmd_so_LDADD = -lpthread -ltelemidiautil -lgingaccsystemthread \
    $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
