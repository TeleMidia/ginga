include $(top_srcdir)/build-aux/Makefile.am.common

AM_CXXFLAGS= $(EXPAT_CFLAGS)
AM_LDFLAGS= $(LTSTATIC) $(EXPAT_LIBS)
noinst_LTLIBRARIES= dataprocessing.la
dataprocessing_la_SOURCES=\
  DataProcessor.cpp\
  EPGProcessor.cpp\
  FilterManager.cpp\
  SectionFilter.cpp\
  dsmcc/Binding.cpp\
  dsmcc/Biop.cpp\
  dsmcc/Crc.cpp\
  dsmcc/DSMCCSectionPayload.cpp\
  dsmcc/DownloadDataBlock.cpp\
  dsmcc/DownloadInfoIndication.cpp\
  dsmcc/DownloadServerInitiate.cpp\
  dsmcc/DsmccMessageHeader.cpp\
  dsmcc/Ior.cpp\
  dsmcc/MessageProcessor.cpp\
  dsmcc/Module.cpp\
  dsmcc/MpegDescriptor.cpp\
  dsmcc/NPTEndpoint.cpp\
  dsmcc/NPTProcessor.cpp\
  dsmcc/NPTReference.cpp\
  dsmcc/Object.cpp\
  dsmcc/ObjectProcessor.cpp\
  dsmcc/ServiceDomain.cpp\
  dsmcc/StreamEvent.cpp\
  dsmcc/StreamMode.cpp\
  dsmcc/TimeBaseClock.cpp\
  ncl/DataFile.cpp\
  ncl/EventDescriptor.cpp\
  ncl/Metadata.cpp\
  ncl/NCLMetadataParser.cpp\
  ncl/NCLSectionProcessor.cpp\
  $(NULL)
