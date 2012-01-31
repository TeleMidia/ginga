
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

if DFBSYSTEM
localLibdir = /usr/local/lib/ginga/mb/dec/

if FUSIONSOUND
libgingaccmbdecdfbfs_so_LDFLAGS = -Wl,-soname,libgingaccmbdecdfbfs.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga \
	-L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

localLib_PROGRAMS = libgingaccmbdecdfbfs.so

libgingaccmbdecdfbfs_so_SOURCES = DFBAudioProvider.cpp \
    FusionSoundAudioProvider.cpp

libgingaccmbdecdfbfs_so_LDADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    -lgingaccmbdfb -lgingaccmb
endif #FUSIONSOUND

if FFMPEGPROVIDER
localLibffmpegadir = /usr/local/lib/ginga/mb/dec/

localLibffmpega_PROGRAMS = libgingaccmbdecdfbffmpega.so

libgingaccmbdecdfbffmpega_so_LDFLAGS = -Wl,-soname,libgingaccmbdecdfbffmpega.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L./ \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbffmpega_so_SOURCES = FFmpegAudioProvider.cpp

libgingaccmbdecdfbffmpega_so_LDADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -lgingaccmbdecdfbfs
    
endif #FFMPEGPROVIDER

endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
