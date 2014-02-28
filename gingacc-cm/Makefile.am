include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(NULL)

AM_CXXFLAGS =
AM_LDFLAGS = $(LTLDFLAGS)

gingacccmlib_LTLIBRARIES =\
  libgingacccm.la\
  libgingacccmassembler.la\
  libgingacccmparser.la\
  libgingacccmcomponent.la\
  $(NULL)

# gingacc-cm
libgingacccm_la_SOURCES =\
  src/ComponentManager.cpp\
  $(NULL)

# gingacc-cm-assembler
libgingacccmassembler_la_SOURCES =\
  src/assembler/ComponentDescription.cpp\
  $(NULL)

# gingacc-cm-component
libgingacccmcomponent_la_SOURCES =\
  src/component/Component.cpp\
  $(NULL)

# gingacc-cm-parser
libgingacccmparser_la_CXXFLAGS = $(AM_CXXFLAGS) $(EXPAT_CFLAGS)
libgingacccmparser_la_LDFLAGS = $(AM_LDFLAGS) $(EXPAT_LIBS)
libgingacccmparser_la_SOURCES =\
  src/parser/ComponentParser.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
gingacccminclude_HEADERS =\
  include/cm/IComponentManager.h\
  $(NULL)

assemblerincludedir = $(gingacccmincludedir)/assembler
assemblerinclude_HEADERS =\
  include/cm/assembler/IComponentDescription.h\
  $(NULL)

componentincludedir = $(gingacccmincludedir)/component
componentinclude_HEADERS =\
  include/cm/component/IComponent.h\
  $(NULL)

parserincludedir = $(gingacccmincludedir)/parser
parserinclude_HEADERS =\
  include/cm/parser/IComponentParser.h\
  $(NULL)
endif

############################## Configuration ###############################

files/componentDescription.xml: files/componentDescription.xml.in
	@$(SED) -e 's:@GINGA_LIBDIR@:$(gingalibdir):' $< >$@

DISTCLEANFILES = files/componentDescription.xml

gingacccmconfdir = $(gingaconfdir)
gingacccmconf_DATA =\
  files/componentDescription.xml\
  $(NULL)
