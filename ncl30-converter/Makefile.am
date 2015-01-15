include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_NCL30_CONVERTER)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = $(LTLDFLAGS)

ncl30converterlib_LTLIBRARIES =\
  libncl30confraxml.la\
  libncl30confrancl.la\
  libncl30connclncm.la\
  $(NULL)

# con-fra-xml
libncl30confraxml_la_CXXFLAGS = $(AM_CXXFLAGS) $(XERCESC_CFLAGS)
libncl30confraxml_la_LDFLAGS = $(AM_LDFLAGS) $(XERCESC_LIBS)
libncl30confraxml_la_SOURCES =\
  src/framework/DOMTreeErrorReporter.cpp\
  src/framework/DocumentParser.cpp\
  src/framework/XMLParsing.cpp\
  $(NULL)

# con-fra-ncl
libncl30confrancl_la_LIBADD =\
  libncl30confraxml.la\
  $(NULL)

libncl30confrancl_la_SOURCES =\
  src/framework/ncl/NclComponentsParser.cpp\
  src/framework/ncl/NclConnectorsParser.cpp\
  src/framework/ncl/NclDocumentParser.cpp\
  src/framework/ncl/NclImportParser.cpp\
  src/framework/ncl/NclInterfacesParser.cpp\
  src/framework/ncl/NclLayoutParser.cpp\
  src/framework/ncl/NclLinkingParser.cpp\
  src/framework/ncl/NclMetainformationParser.cpp\
  src/framework/ncl/NclPresentationControlParser.cpp\
  src/framework/ncl/NclPresentationSpecificationParser.cpp\
  src/framework/ncl/NclStructureParser.cpp\
  src/framework/ncl/NclTransitionParser.cpp\
  $(NULL)

# con-ncl-ncm
libncl30connclncm_la_LIBADD =\
  $(XERCESC_LIBS)\
  libncl30confrancl.la\
  $(NULL)

libncl30connclncm_la_SOURCES =\
  src/ncl/NclComponentsConverter.cpp\
  src/ncl/NclConnectorsConverter.cpp\
  src/ncl/NclDocumentConverter.cpp\
  src/ncl/NclImportConverter.cpp\
  src/ncl/NclInterfacesConverter.cpp\
  src/ncl/NclLayoutConverter.cpp\
  src/ncl/NclLinkingConverter.cpp\
  src/ncl/NclMetainformationConverter.cpp\
  src/ncl/NclPresentationControlConverter.cpp\
  src/ncl/NclPresentationSpecConverter.cpp\
  src/ncl/NclStructureConverter.cpp\
  src/ncl/NclTransitionConverter.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
ncl30converterinclude_HEADERS =\
  include/converter/IDocumentConverter.h\
  $(NULL)

frameworkincludedir = $(ncl30converterincludedir)/framework
frameworkinclude_HEADERS =\
  include/converter/framework/DOMTreeErrorReporter.h\
  include/converter/framework/DocumentParser.h\
  include/converter/framework/ModuleParser.h\
  include/converter/framework/XMLParsing.h\
  $(NULL)

frameworknclincludedir = $(frameworkincludedir)/ncl
frameworknclinclude_HEADERS =\
  include/converter/framework/ncl/NclComponentsParser.h\
  include/converter/framework/ncl/NclConnectorsParser.h\
  include/converter/framework/ncl/NclDocumentParser.h\
  include/converter/framework/ncl/NclImportParser.h\
  include/converter/framework/ncl/NclInterfacesParser.h\
  include/converter/framework/ncl/NclLayoutParser.h\
  include/converter/framework/ncl/NclLinkingParser.h\
  include/converter/framework/ncl/NclMetainformationParser.h\
  include/converter/framework/ncl/NclPresentationControlParser.h\
  include/converter/framework/ncl/NclPresentationSpecificationParser.h\
  include/converter/framework/ncl/NclStructureParser.h\
  include/converter/framework/ncl/NclTransitionParser.h\
  $(NULL)

converternclincludedir = $(ncl30converterincludedir)/ncl
converternclinclude_HEADERS =\
  include/converter/ncl/NclComponentsConverter.h\
  include/converter/ncl/NclConnectorsConverter.h\
  include/converter/ncl/NclDocumentConverter.h\
  include/converter/ncl/NclImportConverter.h\
  include/converter/ncl/NclInterfacesConverter.h\
  include/converter/ncl/NclLayoutConverter.h\
  include/converter/ncl/NclLinkingConverter.h\
  include/converter/ncl/NclMetainformationConverter.h\
  include/converter/ncl/NclPresentationControlConverter.h\
  include/converter/ncl/NclPresentationSpecConverter.h\
  include/converter/ncl/NclStructureConverter.h\
  include/converter/ncl/NclTransitionConverter.h\
  $(NULL)
endif
