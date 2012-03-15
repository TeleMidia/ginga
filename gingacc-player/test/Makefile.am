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
else #AWESOMIUMLIB
noinst_PROGRAMS       = \
                        testAnime \
                        testAudio \
                        testImage \
                        testLinks \
                        testText \
                        testVideo
endif #AWESOMIUMLIB

#testAnime
testAnime_SOURCES     = \
                        testAnime.cpp

testAnime_CFLAGS      = \
                        $(top_srcdir)/include

testAnime_LDADD       = \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccplaani \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ -L/usr/lib/ \
                        -L$(prefix)/lib/ \
                        -L/usr/lib/awesomium-1.6.3 \
                        -L$(prefix)/lib/ginga \
                        -L$(prefix)/lib/ginga/players \
                        -L../src/ \
                        -L../src/image

#testAudio
testAudio_SOURCES     = \
                        testAudio.cpp

testAudio_CFLAGS      = \
                        $(top_srcdir)/include

testAudio_LDADD       = \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ -L/usr/lib/ \
                        -L$(prefix)/lib/ \
                        -L../src/

if AWESOMIUMLIB
#testAwesomium
testAwesomium_SOURCES = \
                        testAwesomium.cpp
                        
testAwesomium_CFLAGS  = \
                        $(top_srcdir)/include
                        
testAwesomium_LDADD   = \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -lawesomium-1.6.3 \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L/usr/lib/awesomium-1.6.3 \
                        -L$(prefix)/lib/ \
                        -L$(prefix)/lib/ginga \
                        -L$(prefix)/lib/ginga/players \
                        -L../src/
endif #AWESOMIUMLIB

#testImage
testImage_SOURCES     = \
                        testImage.cpp
                        
testImage_CFLAGS      = \
                        $(top_srcdir)/include
                        
testImage_LDADD       = \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L$(prefix)/lib/ \
                        -L../src/

#testLinks
testLinks_SOURCES     = \
                        testLinks.cpp

testLinks_CFLAGS      = \
                        $(top_srcdir)/include

testLinks_LDADD       = \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L$(prefix)/lib/ \
                        -L../src/

#testText
testText_SOURCES      = \
                        testText.cpp

testText_CFLAGS       = \
                        $(top_srcdir)/include

testText_LDADD        = \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L$(prefix)/lib/ \
                        -L../src/

#testVideo
testVideo_SOURCES     = \
                        testVideo.cpp

testVideo_CFLAGS      = \
                        $(top_srcdir)/include

testVideo_LDADD       = \
                        -lpthread \
                        -ltelemidiautil \
                        -lgingaccplayer \
                        -lgingaccmb \
                        -lgingaccsystem \
                        -L/lib/ \
                        -L/usr/lib/ \
                        -L$(prefix)/lib/ \
                        -L../src/

MAINTAINERCLEANFILES = Makefile.in
