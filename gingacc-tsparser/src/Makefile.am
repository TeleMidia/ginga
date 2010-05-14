
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ $(all_includes)

LIBS =

localLibdir = /usr/local/lib/ginga

libgingacctsparser_so_LDFLAGS = -Wl,-soname,libgingacctsparser.so \
	-shared -L/usr/local/lib -L/usr/local/lib/ginga

localLib_PROGRAMS = libgingacctsparser.so
libgingacctsparser_so_SOURCES = Demuxer.cpp FrontendFilter.cpp \
    PipeFilter.cpp PSIFilter.cpp \
    PesFilter.cpp TransportSection.cpp TSPacket.cpp \
    si/AIT.cpp si/Application.cpp si/EventInfo.cpp \
    si/Pat.cpp si/Pmt.cpp si/ServiceInfo.cpp \
    si/descriptors/ApplicationDescriptor.cpp \
    si/descriptors/ApplicationLocationDescriptor.cpp \
    si/descriptors/ApplicationNameDescriptor.cpp \
	si/descriptors/ExtendedEventDescriptor.cpp \
	si/descriptors/PrefetchDescriptor.cpp \
	si/descriptors/TransportProtocolDescriptor.cpp \
	si/descriptors/LogoTransmissionDescriptor.cpp \
	si/descriptors/ShortEventDescriptor.cpp \
	si/descriptors/ComponentDescriptor.cpp \
	si/descriptors/AudioComponentDescriptor.cpp \
	si/descriptors/ParentalRatingDescriptor.cpp \
	si/descriptors/SeriesDescriptor.cpp \
	si/descriptors/DataContentDescriptor.cpp \
	si/descriptors/ContentDescriptor.cpp \
	si/descriptors/DigitalCCDescriptor.cpp \
	si/descriptors/ContentAvailabilityDescriptor.cpp \
	si/descriptors/ServiceDescriptor.cpp

libgingacctsparser_so_LDADD = -ltelemidiautil -lpthread -lgingaccsystemthread \
    -lgingacctuner
