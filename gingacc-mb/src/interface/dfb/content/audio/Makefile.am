
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/mb/dec

lib_LTLIBRARIES = 
if DFBSYSTEM

if FUSIONSOUND
lib_LTLIBRARIES += libgingaccmbdecdfbfs.la

libgingaccmbdecdfbfs_la_SOURCES = DFBAudioProvider.cpp \
    FusionSoundAudioProvider.cpp

libgingaccmbdecdfbfs_la_LIBADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    -lgingaccmbdfb -lgingaccmb \
    -L/usr/local/lib -L/usr/local/lib/ginga \
	-L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb
    
endif #FUSIONSOUND

if FFMPEGPROVIDER
lib_LTLIBRARIES += libgingaccmbdecdfbffmpega.la

libgingaccmbdecdfbffmpega_la_SOURCES = FFmpegAudioProvider.cpp

libgingaccmbdecdfbffmpega_la_LIBADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -lgingaccmbdecdfbfs \
    -L/usr/local/lib -L/usr/local/lib/ginga -L./ \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb
    
endif #FFMPEGPROVIDER

endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
