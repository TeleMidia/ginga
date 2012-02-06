
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(FFMPEG_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

if DFBSYSTEM
localLibdfbdir = /usr/local/lib/ginga/mb/dec/

localLibdfb_PROGRAMS = libgingaccmbdecdfbav.so

libgingaccmbdecdfbav_so_LDFLAGS = -Wl,-soname,libgingaccmbdecdfbav.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbav_so_SOURCES = DFBVideoProvider.cpp

libgingaccmbdecdfbav_so_LDADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem

if XINEPROVIDER
localLibxinedir = /usr/local/lib/ginga/mb/dec/

localLibxine_PROGRAMS = libgingaccmbdecdfbxineav.so

libgingaccmbdecdfbxineav_so_LDFLAGS = -Wl,-soname,libgingaccmbdecdfbxineav.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbxineav_so_SOURCES = XineVideoProvider.cpp

libgingaccmbdecdfbxineav_so_LDADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem -lxine

endif #XINEPROVIDER

if FFMPEGPROVIDER
localLibffmpegavdir = /usr/local/lib/ginga/mb/dec/

localLibffmpegav_PROGRAMS = libgingaccmbdecdfbffmpegav.so

libgingaccmbdecdfbffmpegav_so_LDFLAGS = -Wl,-soname,libgingaccmbdecdfbffmpegav.so \
    -shared -L/usr/local/lib -L/usr/local/lib/ginga -L../audio/ \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbffmpegav_so_SOURCES = FFmpegVideoProvider.cpp

libgingaccmbdecdfbffmpegav_so_LDADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -lgingaccmbdfbdecfs

endif #FFMPEGPROVIDER
endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
