
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ -I$(prefix)/include/directfb/ \
	$(FFMPEG_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/mb/dec

if DFBSYSTEM
lib_LTLIBRARIES = libgingaccmbdecdfbav.la

libgingaccmbdecdfbav_la_SOURCES = DFBVideoProvider.cpp

libgingaccmbdecdfbav_la_LIBADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -L$(prefix)/lib -L$(prefix)/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbav_la_LDFLAGS = \
                                  -no-undefined \
                                  -version-info 13:1:8

if XINEPROVIDER
lib_LTLIBRARIES += libgingaccmbdecdfbxineav.la

libgingaccmbdecdfbxineav_la_SOURCES = XineVideoProvider.cpp

libgingaccmbdecdfbxineav_la_LIBADD = $(DFB_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem -lxine \
    -L$(prefix)/lib -L$(prefix)/lib/ginga \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbxineav_la_LDFLAGS = \
                                      -no-undefined \
                                      -version-info 13:1:8

endif #XINEPROVIDER

if FFMPEGPROVIDER
lib_LTLIBRARIES += libgingaccmbdecdfbffmpegav.la

libgingaccmbdecdfbffmpegav_la_SOURCES = FFmpegVideoProvider.cpp

libgingaccmbdecdfbffmpegav_la_LIBADD = $(DFB_LIBS) $(FUSIONSOUND_LIBS) \
    $(FFMPEG_LIBS) $(DVC_LIBS) -ltelemidiautil \
    -lgingaccmbdfb -lgingaccmb -lgingaccsystem \
    -lgingaccmbdfbdecfs \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../audio/ \
    -L$(top_srcdir)/src -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbffmpegav_la_LDFLAGS = \
                                        -no-undefined \
                                        -version-info 13:1:8

endif #FFMPEGPROVIDER
endif #DFBSYSTEM

MAINTAINERCLEANFILES = Makefile.in
