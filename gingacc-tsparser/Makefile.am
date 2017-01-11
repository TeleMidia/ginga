include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_TUNER)\
  $(INCLUDE_GINGACC_TSPARSER)\
  $(NULL)

AM_CXXFLAGS=
AM_LDFLAGS= $(LTSTATIC) $(PTHREAD_LDFLAGS)

noinst_LTLIBRARIES = tsparser.la

tsparser_la_SOURCES =\
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
