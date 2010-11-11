
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(FFMPEG_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

localLibdfbdir = /usr/local/lib/ginga/iocontents/

localLibdfb_PROGRAMS = libgingaccsysctsdfbav.so

libgingaccsysctsdfbav_so_LDFLAGS = -Wl,-soname,libgingaccsysctsdfbav.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L../../../ \
    -L../../../../thread

libgingaccsysctsdfbav_so_SOURCES = DFBVideoProvider.cpp

libgingaccsysctsdfbav_so_LDADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccsystemiodfb -lgingaccsystemio -lgingaccsystemthread

if XINEPROVIDER
localLibxinedir = /usr/local/lib/ginga/iocontents/

localLibxine_PROGRAMS = libgingaccsysctsxineav.so

libgingaccsysctsxineav_so_LDFLAGS = -Wl,-soname,libgingaccsysctsxineav.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L../../../ \
    -L../../../../thread

libgingaccsysctsxineav_so_SOURCES = XineVideoProvider.cpp

libgingaccsysctsxineav_so_LDADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccsystemiodfb -lgingaccsystemio -lgingaccsystemthread -lxine

endif

if FFMPEGPROVIDER
localLibffmpegavdir = /usr/local/lib/ginga/iocontents/

localLibffmpegav_PROGRAMS = libgingaccsysctsffmpegav.so

libgingaccsysctsffmpegav_so_LDFLAGS = -Wl,-soname,libgingaccsysctsffmpegav.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L../../../ \
    -L../audio/ -L../../../../thread

libgingaccsysctsffmpegav_so_SOURCES = FFmpegVideoProvider.cpp

libgingaccsysctsffmpegav_so_LDADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccsystemiodfb -lgingaccsystemio -lgingaccsystemthread \
    -lgingaccsysctsfs

endif
