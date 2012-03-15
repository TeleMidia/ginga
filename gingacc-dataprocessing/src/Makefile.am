
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ $(all_includes) -I$(top_srcdir)/include

LIBS =

# the library search path.
libdir = $(prefix)/lib/ginga

libgingaccdataprocessing_so_CPPFLAGS = -I../include

lib_LTLIBRARIES = libgingaccdataprocessing.la
libgingaccdataprocessing_la_SOURCES = DataProcessor.cpp EPGProcessor.cpp \
    FilterManager.cpp SectionFilter.cpp \
    dsmcc/StreamEvent.cpp dsmcc/carousel/Binding.cpp dsmcc/carousel/Biop.cpp \
    dsmcc/carousel/Ior.cpp dsmcc/carousel/ServiceDomain.cpp \
    dsmcc/carousel/data/DownloadDataBlock.cpp \
    dsmcc/carousel/data/DownloadInfoIndication.cpp \
    dsmcc/carousel/data/DownloadServerInitiate.cpp \
    dsmcc/carousel/data/DsmccMessageHeader.cpp \
    dsmcc/carousel/data/MessageProcessor.cpp \
    dsmcc/carousel/data/Module.cpp \
    dsmcc/carousel/object/Object.cpp \
    dsmcc/carousel/object/ObjectProcessor.cpp \
    dsmcc/npt/Crc.cpp dsmcc/npt/Descriptor.cpp \
    dsmcc/npt/DSMCCSectionPayload.cpp dsmcc/npt/NPTProcessor.cpp \
    dsmcc/npt/NPTReference.cpp dsmcc/npt/Stc.cpp dsmcc/npt/StreamMode.cpp \
    dsmcc/npt/TimeBaseClock.cpp

libgingaccdataprocessing_la_LIBADD = -ltelemidiautil -lgingaccsystem \
    $(COMPONENTS_LIBS) -L$(prefix)/lib/ -L$(prefix)/lib/ginga/

libgingaccdataprocessing_la_LDFLAGS = \
                                      -no-undefined \
                                      -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
