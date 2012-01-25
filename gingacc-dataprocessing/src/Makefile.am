
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ $(all_includes) -I$(top_srcdir)/include

LIBS =

# the library search path.
localLibdir = /usr/local/lib/ginga

libgingaccdataprocessing_so_CPPFLAGS = -I../include
libgingaccdataprocessing_so_LDFLAGS = -Wl,-soname,libgingaccdataprocessing.so \
	-shared -L/usr/local/lib -L/usr/local/lib/ginga

localLib_PROGRAMS = libgingaccdataprocessing.so
libgingaccdataprocessing_so_SOURCES = DataProcessor.cpp EPGProcessor.cpp \
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

libgingaccdataprocessing_so_LDADD = -ltelemidiautil -lgingaccsystem \
    $(COMPONENTS_LIBS) -L/usr/local/lib/ -L/usr/local/lib/ginga/

MAINTAINERCLEANFILES = Makefile.in
