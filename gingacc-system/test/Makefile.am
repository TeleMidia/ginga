
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
    $(FFMPEG_CFLAGS) $(all_includes)

LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD = -ltelemidiautil -lgingaccsystemthread -lgingaccsystemio \
    -lgingaccsystemiodfb -lgingaccsystemiocodemap -lgingaccsysctsimgdfb \
    -lgingaccsysctsxineav -lgingaccsysctsdfbav -lgingaccsysctsfs \
    -lgingaccsysctsffmpegav -lgingaccsysctsffmpega \
    -lxine $(FUSIONSOUND_LIBS) $(FFMPEG_LIBS)

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L../src/thread -L../src/io \
    -L../src/io/interface/content/audio -L../src/io/interface/content/image \
    -L../src/io/interface/content/text -L../src/io/interface/content/video
