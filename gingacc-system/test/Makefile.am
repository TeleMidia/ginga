
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
    $(FFMPEG_CFLAGS) $(all_includes)

LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

TESTLIBS = -ltelemidiautil -lgingaccsystemthread -lgingaccsystemio \
    -lgingaccsystemiodfb -lgingaccsystemiocodemap -lgingaccsysctsimgdfb \
    -lgingaccsysctsdfbav \
    $(XINE_LIBS) $(FUSIONSOUND_LIBS) $(FFMPEG_LIBS)

if FUSIONSOUND
TESTLIBS += -lgingaccsysctsfs 
endif

if XINEPROVIDER
TESTLIBS += -lgingaccsysctsxineav 
endif

if FFMPEGPROVIDER
TESTLIBS += -lgingaccsysctsffmpegav -lgingaccsysctsffmpega
endif

test_LDADD = $(TESTLIBS)

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L../src/thread -L../src/io \
    -L../src/io/interface/content/audio -L../src/io/interface/content/image \
    -L../src/io/interface/content/text -L../src/io/interface/content/video
