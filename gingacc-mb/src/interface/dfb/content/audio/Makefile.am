
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ -I$(prefix)/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/mb/dec

lib_LTLIBRARIES = 
if DFBSYSTEM

if FUSIONSOUND
lib_LTLIBRARIES += libgingaccmbdecdfbfs.la

libgingaccmbdecdfbfs_la_SOURCES = DFBAudioProvider.cpp \
    FusionSoundAudioProvider.cpp

libgingaccmbdecdfbfs_la_LIBADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    -lgingaccmbdfb -lgingaccmb \
    -L$(prefix)/lib -L$(prefix)/lib/ginga \
	-L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbfs_la_LDFLAGS = \
                                  -no-undefined \
                                  -version-info 13:1:8

endif #FUSIONSOUND

if FFMPEGPROVIDER
lib_LTLIBRARIES += libgingaccmbdecdfbffmpega.la

libgingaccmbdecdfbffmpega_la_SOURCES = FFmpegAudioProvider.cpp

libgingaccmbdecdfbffmpega_la_LIBADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -lgingaccmbdecdfbfs \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L./ \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbffmpega_la_LDFLAGS = \
                                       -no-undefined \
                                       -version-info 13:1:8

endif #FFMPEGPROVIDER

endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
