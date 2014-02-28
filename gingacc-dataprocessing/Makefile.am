include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_TUNER)\
  $(INCLUDE_GINGACC_TSPARSER)\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LDFLAGS)

gingaccdataprocessinglib_LTLIBRARIES =\
  libgingaccdataprocessing.la\
  libgingaccdataprocncl.la\
  libgingaccdataprocncledit.la\
  $(NULL)

# gingacc-data-processing
libgingaccdataprocessing_la_SOURCES =\
  src/DataProcessor.cpp\
  src/EPGProcessor.cpp\
  src/FilterManager.cpp\
  src/SectionFilter.cpp\
  src/dsmcc/StreamEvent.cpp\
  src/dsmcc/carousel/Binding.cpp\
  src/dsmcc/carousel/Biop.cpp\
  src/dsmcc/carousel/Ior.cpp\
  src/dsmcc/carousel/ServiceDomain.cpp\
  src/dsmcc/carousel/data/DownloadDataBlock.cpp\
  src/dsmcc/carousel/data/DownloadInfoIndication.cpp\
  src/dsmcc/carousel/data/DownloadServerInitiate.cpp\
  src/dsmcc/carousel/data/DsmccMessageHeader.cpp\
  src/dsmcc/carousel/data/MessageProcessor.cpp\
  src/dsmcc/carousel/data/Module.cpp\
  src/dsmcc/carousel/object/Object.cpp\
  src/dsmcc/carousel/object/ObjectProcessor.cpp\
  src/dsmcc/npt/Crc.cpp\
  src/dsmcc/npt/DSMCCSectionPayload.cpp\
  src/dsmcc/npt/MpegDescriptor.cpp\
  src/dsmcc/npt/NPTEndpoint.cpp\
  src/dsmcc/npt/NPTProcessor.cpp\
  src/dsmcc/npt/NPTReference.cpp\
  src/dsmcc/npt/StreamMode.cpp\
  src/dsmcc/npt/TimeBaseClock.cpp\
  $(NULL)

# gingacc-data-proc-ncl
libgingaccdataprocncl_la_CXXFLAGS = $(AM_CXXFLAGS) $(EXPAT_CFLAGS)
libgingaccdataprocncl_la_LDFLAGS = $(EXPAT_LIBS)
libgingaccdataprocncl_la_SOURCES =\
  src/ncl/Metadata.cpp\
  src/ncl/DataFile.cpp\
  src/ncl/NCLSectionProcessor.cpp\
  src/ncl/parser/NCLMetadataParser.cpp\
  ($NULL)

# gingacc-data-proc-ncl-edit
libgingaccdataprocncledit_la_SOURCES = src/ncl/edit/EventDescriptor.cpp

################################# Headers ##################################
if INSTALL_HEADERS
gingaccdataprocessinginclude_HEADERS =\
  include/dataprocessing/IDataProcessor.h\
  include/dataprocessing/IEPGListener.h\
  include/dataprocessing/IEPGProcessor.h\
  $(NULL)

dsmccincludedir = $(gingaccdataprocessingincludedir)/dsmcc
dsmccinclude_HEADERS =\
  include/dataprocessing/dsmcc/IStreamEvent.h\
  include/dataprocessing/dsmcc/IStreamEventListener.h\
  $(NULL)

objectincludedir = $(dsmccincludedir)/carousel/object
objectinclude_HEADERS =\
  include/dataprocessing/dsmcc/carousel/object/IObjectListener.h\
  $(NULL)

nclincludedir = $(gingaccdataprocessingincludedir)/ncl
nclinclude_HEADERS =\
  include/dataprocessing/ncl/IDataFile.h\
  include/dataprocessing/ncl/IMetadata.h\
  include/dataprocessing/ncl/INCLStructure.h\
  include/dataprocessing/ncl/INCLSectionProcessor.h\
  $(NULL)

editincludedir = $(nclincludedir)/edit
editinclude_HEADERS =\
  include/dataprocessing/ncl/edit/EventDescriptor.h\
  $(NULL)
endif
