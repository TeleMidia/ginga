
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
    $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga

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
    -L/lib/ -L/usr/lib/ -L$(prefix)/lib/ \
    -L$(prefix)/lib/ginga

libgingaccmd_la_LDFLAGS = \
                          -no-undefined \
                          -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
