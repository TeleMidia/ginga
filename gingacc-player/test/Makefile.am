
# set the include path found by configure
INCLUDES              = \
                        $(all_includes) \
                        -I$(top_srcdir) \
                        -I$(top_srcdir)/include \
                        $(COMPONENTS_INCS) \
                        -I/usr/include/Awesomium/

LIBS =

if AWESOMIUMLIB
noinst_PROGRAMS       = \
                        testAnime \
                        testAudio \
                        testAwesomium \
                        testImage \
                        testLinks \
                        testText \
                        testVideo
else
noinst_PROGRAMS       = \
                        testAnime \
                        testAudio \
                        testImage \
                        testLinks \
                        testText \
                        testVideo
endif

#testAnime
testAnime_SOURCES     = \
                        testAnime.cpp

testAnime_CFLAGS      = \
                        $(top_srcdir)/include

testAnime_LDADD       = \
                        -ldl \
                        -lawesomium-1.6.3 \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccplaani \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ -L/usr/lib/ \
                        -L/usr/local/lib/ \
                        -L/usr/lib/awesomium-1.6.3 \
                        -L/usr/local/lib/ginga \
                        -L/usr/local/lib/ginga/players \
                        -L../src/ \
                        -L../src/image

#testAudio
testAudio_SOURCES     = \
                        testAudio.cpp

testAudio_CFLAGS      = \
                        $(top_srcdir)/include

testAudio_LDADD       = \
                        -ldl \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ -L/usr/lib/ \
                        -L/usr/local/lib/ \
                        -L../src/

if AWESOMIUMLIB
#testAwesomium
testAwesomium_SOURCES = \
                        testAwesomium.cpp
                        
testAwesomium_CFLAGS  = \
                        $(top_srcdir)/include
                        
testAwesomium_LDADD   = \
                        -ldl \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L/usr/local/lib/ \
                        -L/usr/local/lib/ginga \
                        -L/usr/local/lib/ginga/players \
                        -L../src/
endif

#testImage
testImage_SOURCES     = \
                        testImage.cpp
                        
testImage_CFLAGS      = \
                        $(top_srcdir)/include
                        
testImage_LDADD       = \
                        -ldl \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L/usr/local/lib/ \
                        -L../src/

#testLinks
testLinks_SOURCES     = \
                        testLinks.cpp

testLinks_CFLAGS      = \
                        $(top_srcdir)/include

testLinks_LDADD       = \
                        -ldl \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L/usr/local/lib/ \
                        -L../src/

#testText
testText_SOURCES      = \
                        testText.cpp

testText_CFLAGS       = \
                        $(top_srcdir)/include

testText_LDADD        = \
                        -ldl \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L/usr/local/lib/ \
                        -L../src/

#testVideo
testVideo_SOURCES     = \
                        testVideo.cpp

testVideo_CFLAGS      = \
                        $(top_srcdir)/include

testVideo_LDADD       = \
                        -ldl \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L/usr/local/lib/ \
                        -L../src/

MAINTAINERCLEANFILES = Makefile.in
