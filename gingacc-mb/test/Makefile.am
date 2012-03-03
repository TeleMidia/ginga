
# set the include path found by configure
INCLUDES             = \
                       -I/usr/local/include/ginga/ \
                       -I$(top_srcdir)/include \
                       $(all_includes)

LIBS                 =

noinst_PROGRAMS      = \
                       testAudio \
                       testFont \
                       testInput \
                       testScreen \
                       testVideo


#testAudio
testAudio_SOURCES    = \
                       testAudio.cpp

testAudio_LDADD      = \
                       -ldl \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb

testAudio_LDFLAGS    = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L/usr/local/lib/ \
                       -L/usr/local/lib/ginga \
                       -L$(top_srcdir)/src

#testFont
testFont_SOURCES    = \
                       testFont.cpp

testFont_LDADD      = \
                       -ldl \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb

testFont_LDFLAGS    = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L/usr/local/lib/ \
                       -L/usr/local/lib/ginga \
                       -L$(top_srcdir)/src

#testInput
testInput_SOURCES    = \
                       testInput.cpp

testInput_LDADD      = \
                       -ldl \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb

testInput_LDFLAGS    = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L/usr/local/lib/ \
                       -L/usr/local/lib/ginga \
                       -L$(top_srcdir)/src

#testScreen
testScreen_SOURCES   = \
                       testScreen.cpp

testScreen_LDADD     = \
                       -ldl \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb

testScreen_LDFLAGS   = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L/usr/local/lib/ \
                       -L/usr/local/lib/ginga \
                       -L$(top_srcdir)/src

#testVideo
testVideo_SOURCES    = \
                       testVideo.cpp

testVideo_LDADD      = \
                       -ldl \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb

testVideo_LDFLAGS    = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L/usr/local/lib/ \
                       -L/usr/local/lib/ginga \
                       -L$(top_srcdir)/src


MAINTAINERCLEANFILES = \
                       Makefile.in
