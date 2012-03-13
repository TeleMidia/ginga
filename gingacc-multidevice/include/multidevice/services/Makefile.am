METASOURCES = AUTO

# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
    $(all_includes) -I$(top_srcdir)/include

LIBS =
servicesIncludedir = $(prefix)/include/ginga/multidevice/services
servicesInclude_HEADERS = IDeviceDomain.h RemoteEventService.h

servicesnetIncludedir = $(prefix)/include/ginga/multidevice/services/network
servicesnetInclude_HEADERS = network/ISocketService.h network/TcpClientConnection.h \
	network/TcpSocketService.h
MAINTAINERCLEANFILES = Makefile.in
