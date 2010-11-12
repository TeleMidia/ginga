
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =
localLibdir = /usr/local/lib/ginga/iocontents/

if FUSIONSOUND
libgingaccsysctsfs_so_LDFLAGS = -Wl,-soname,libgingaccsysctsfs.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../../

localLib_PROGRAMS = libgingaccsysctsfs.so

libgingaccsysctsfs_so_SOURCES = DFBAudioProvider.cpp \
    FusionSoundAudioProvider.cpp

libgingaccsysctsfs_so_LDADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    -lgingaccsystemiodfb -lgingaccsystemio
endif

if FFMPEGPROVIDER
localLibffmpegadir = /usr/local/lib/ginga/iocontents/

localLibffmpega_PROGRAMS = libgingaccsysctsffmpega.so

libgingaccsysctsffmpega_so_LDFLAGS = -Wl,-soname,libgingaccsysctsffmpega.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L../../../ \
    -L./ -L../../../../thread

libgingaccsysctsffmpega_so_SOURCES = FFmpegAudioProvider.cpp

libgingaccsysctsffmpega_so_LDADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccsystemiodfb -lgingaccsystemio -lgingaccsystemthread \
    -lgingaccsysctsfs
endif
MAINTAINERCLEANFILES = Makefile.in
