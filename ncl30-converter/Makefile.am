include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_NCL30)\
  $(INCLUDE_NCL30_CONVERTER)\
  $(NULL)

libncl30confrancl_la_CXXFLAGS = $(AM_CXXFLAGS) $(XERCESC_CFLAGS)
libncl30confrancl_la_LDFLAGS = $(AM_LDFLAGS) $(XERCESC_LIBS)

ncl30converterlib_LTLIBRARIES =\
  libncl30confrancl.la\
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

libncl30confrancl_la_SOURCES +=\
  src/framework/DOMTreeErrorReporter.cpp\
  src/framework/DocumentParser.cpp\
  src/framework/XMLParsing.cpp\
  $(NULL)

libncl30confrancl_la_SOURCES +=\
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
