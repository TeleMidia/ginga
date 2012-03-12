
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
    $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga

lib_LTLIBRARIES = libgingaccmd.la

libgingaccmd_la_SOURCES = RemoteDeviceManager.cpp RemoteDevice.cpp \
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

libgingaccmd_la_LIBADD = -lpthread -ltelemidiautil -lgingaccsystem \
    $(COMPONENTS_LIBS) \
    -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga

MAINTAINERCLEANFILES = Makefile.in
