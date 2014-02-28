include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_TUNER)\
  $(INCLUDE_GINGACC_TSPARSER)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS)

gingacctsparserlib_LTLIBRARIES = libgingacctsparser.la

libgingacctsparser_la_SOURCES =\
  src/Demuxer.cpp\
  src/FrontendFilter.cpp\
  src/PipeFilter.cpp\
  src/PSIFilter.cpp\
  src/PesFilter.cpp\
  src/TransportSection.cpp\
  src/TSAdaptationField.cpp\
  src/TSPacket.cpp\
  src/si/AIT.cpp\
  src/si/Application.cpp\
  src/si/EventInfo.cpp\
  src/si/Pat.cpp\
  src/si/Pmt.cpp\
  src/si/ServiceInfo.cpp\
  src/si/TOT.cpp\
  src/si/descriptors/ApplicationDescriptor.cpp\
  src/si/descriptors/ApplicationLocationDescriptor.cpp\
  src/si/descriptors/ApplicationNameDescriptor.cpp\
  src/si/descriptors/ExtendedEventDescriptor.cpp\
  src/si/descriptors/PrefetchDescriptor.cpp\
  src/si/descriptors/TransportProtocolDescriptor.cpp\
  src/si/descriptors/LogoTransmissionDescriptor.cpp\
  src/si/descriptors/ShortEventDescriptor.cpp\
  src/si/descriptors/ComponentDescriptor.cpp\
  src/si/descriptors/AudioComponentDescriptor.cpp\
  src/si/descriptors/ParentalRatingDescriptor.cpp\
  src/si/descriptors/SeriesDescriptor.cpp\
  src/si/descriptors/DataContentDescriptor.cpp\
  src/si/descriptors/ContentDescriptor.cpp\
  src/si/descriptors/DigitalCCDescriptor.cpp\
  src/si/descriptors/ContentAvailabilityDescriptor.cpp\
  src/si/descriptors/ServiceDescriptor.cpp\
  src/si/descriptors/LocalTimeOffsetDescriptor.cpp\
  src/si/descriptors/StreamIdentifierDescriptor.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
gingacctsparserinclude_HEADERS =\
  include/tsparser/IAIT.h\
  include/tsparser/IApplication.h\
  include/tsparser/IDemuxer.h\
  include/tsparser/IEventInfo.h\
  include/tsparser/IFilterListener.h\
  include/tsparser/IServiceInfo.h\
  include/tsparser/IMpegDescriptor.h\
  include/tsparser/ITSPacket.h\
  include/tsparser/ITSFilter.h\
  include/tsparser/IShortEventDescriptor.h\
  include/tsparser/ITransportSection.h\
  include/tsparser/ShortEventDescriptor.h\
  include/tsparser/ExtendedEventDescriptor.h\
  include/tsparser/AudioComponentDescriptor.h\
  include/tsparser/ParentalRatingDescriptor.h\
  include/tsparser/ComponentDescriptor.h\
  include/tsparser/SeriesDescriptor.h\
  include/tsparser/DataContentDescriptor.h\
  include/tsparser/ContentDescriptor.h\
  include/tsparser/DigitalCCDescriptor.h\
  include/tsparser/ContentAvailabilityDescriptor.h\
  include/tsparser/ServiceDescriptor.h\
  include/tsparser/IServiceDescriptor.h\
  include/tsparser/IExtendedEventDescriptor.h\
  include/tsparser/IParentalRatingDescriptor.h\
  include/tsparser/ApplicationDescriptor.h\
  include/tsparser/ApplicationLocationDescriptor.h\
  include/tsparser/ApplicationNameDescriptor.h\
  include/tsparser/PrefetchDescriptor.h\
  include/tsparser/TransportProtocolDescriptor.h\
  include/tsparser/LocalTimeOffsetDescriptor.h\
  include/tsparser/ITOT.h\
  include/tsparser/TOT.h\
  $(NULL)
endif
