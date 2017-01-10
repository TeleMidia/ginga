include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS)

gingaccmultidevicelib_LTLIBRARIES = libgingaccmd.la

libgingaccmd_la_SOURCES =\
  src/RemoteDeviceManager.cpp\
  src/RemoteDevice.cpp\
  src/services/RemoteEventService.cpp\
  src/services/DeviceDomain.cpp\
  src/services/BaseDeviceDomain.cpp\
  src/services/ActiveDeviceDomain.cpp\
  src/services/PassiveDeviceDomain.cpp\
  src/services/device/DeviceService.cpp\
  src/services/device/BaseDeviceService.cpp\
  src/services/device/ActiveDeviceService.cpp\
  src/services/device/PassiveDeviceService.cpp\
  src/services/network/NetworkUtil.cpp\
  src/services/network/BroadcastSocketService.cpp\
  src/services/network/MulticastSocketService.cpp\
  src/services/network/BroadcastDualSocketService.cpp\
  src/services/network/TcpSocketService.cpp\
  src/services/network/TcpClientConnection.cpp\
  $(NULL)
