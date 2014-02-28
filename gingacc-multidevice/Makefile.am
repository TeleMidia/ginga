include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
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

################################# Headers ##################################
if INSTALL_HEADERS
gingaccmultideviceinclude_HEADERS =\
  include/multidevice/IRemoteDeviceManager.h\
  include/multidevice/IRemoteDeviceListener.h\
  include/multidevice/IRemoteDevice.h\
  $(NULL)

servicesincludedir = $(gingaccmultideviceincludedir)/services
servicesinclude_HEADERS =\
  include/multidevice/services/IDeviceDomain.h\
  include/multidevice/services/RemoteEventService.h\
  $(NULL)

networkincludedir = $(servicesincludedir)/network
networkinclude_HEADERS =\
  include/multidevice/services/network/ISocketService.h\
  include/multidevice/services/network/TcpClientConnection.h\
  include/multidevice/services/network/TcpSocketService.h\
  $(NULL)
endif
