include $(top_srcdir)/build-aux/Makefile.am.common

AM_CXXFLAGS= $(XERCESC_CFLAGS)
AM_LDFLAGS= $(LTSTATIC) $(XERCESC_LIBS)
noinst_LTLIBRARIES=  nclconv.la

nclconv_la_SOURCES=\
  DOMTreeErrorReporter.cpp\
  DocumentParser.cpp\
  NclComponentsConverter.cpp\
  NclComponentsParser.cpp\
  NclConnectorsConverter.cpp\
  NclConnectorsParser.cpp\
  NclDocumentConverter.cpp\
  NclDocumentParser.cpp\
  NclImportConverter.cpp\
  NclImportParser.cpp\
  NclInterfacesConverter.cpp\
  NclInterfacesParser.cpp\
  NclLayoutConverter.cpp\
  NclLayoutParser.cpp\
  NclLinkingConverter.cpp\
  NclLinkingParser.cpp\
  NclMetainformationConverter.cpp\
  NclMetainformationParser.cpp\
  NclPresentationControlConverter.cpp\
  NclPresentationControlParser.cpp\
  NclPresentationSpecConverter.cpp\
  NclPresentationSpecificationParser.cpp\
  NclStructureConverter.cpp\
  NclStructureParser.cpp\
  NclTransitionConverter.cpp\
  NclTransitionParser.cpp\
  XMLParsing.cpp\
  $(NULL)
