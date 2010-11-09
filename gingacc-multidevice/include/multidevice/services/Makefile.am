METASOURCES = AUTO

# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
    $(all_includes) -I$(top_srcdir)/include

LIBS =
servicesIncludedir = /usr/local/include/ginga/multidevice/services
servicesInclude_HEADERS = IDeviceDomain.h RemoteEventService.h

servicesnetIncludedir = /usr/local/include/ginga/multidevice/services/network
servicesnetInclude_HEADERS = network/ISocketService.h network/TcpClientConnection.h \
	network/TcpSocketService.h