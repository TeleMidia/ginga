
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga

lib_LTLIBRARIES = libgingacctsparser.la
libgingacctsparser_la_SOURCES = Demuxer.cpp FrontendFilter.cpp \
    PipeFilter.cpp PSIFilter.cpp \
    PesFilter.cpp TransportSection.cpp TSPacket.cpp \
    si/AIT.cpp si/Application.cpp si/EventInfo.cpp \
    si/Pat.cpp si/Pmt.cpp si/ServiceInfo.cpp si/TOT.cpp \
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
	si/descriptors/ServiceDescriptor.cpp \
	si/descriptors/LocalTimeOffsetDescriptor.cpp

libgingacctsparser_la_LIBADD = -ltelemidiautil -lpthread -lgingaccsystem \
    -lgingacctuner \
    -L$(prefix)/lib -L$(prefix)/lib/ginga

libgingacctsparser_la_LDFLAGS = \
                                -no-undefined \
                                -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
