
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(FFMPEG_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/mb/dec

if DFBSYSTEM
lib_LTLIBRARIES = libgingaccmbdecdfbav.la

libgingaccmbdecdfbav_la_SOURCES = DFBVideoProvider.cpp

libgingaccmbdecdfbav_la_LIBADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

if XINEPROVIDER
lib_LTLIBRARIES += libgingaccmbdecdfbxineav.la

libgingaccmbdecdfbxineav_la_SOURCES = XineVideoProvider.cpp

libgingaccmbdecdfbxineav_la_LIBADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem -lxine \
    -L/usr/local/lib -L/usr/local/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

endif #XINEPROVIDER

if FFMPEGPROVIDER
lib_LTLIBRARIES += libgingaccmbdecdfbffmpegav.la

libgingaccmbdecdfbffmpegav_la_SOURCES = FFmpegVideoProvider.cpp

libgingaccmbdecdfbffmpegav_la_LIBADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -lgingaccmbdfbdecfs \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../audio/ \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

endif #FFMPEGPROVIDER
endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
