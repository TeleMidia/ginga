
# set the include path found by configure
INCLUDES             = \
                       -I$(prefix)/include/ginga/ \
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
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb \
                       $(TEST_LIBS)

testAudio_LDFLAGS    = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L$(prefix)/lib/ginga \
                       -L$(top_srcdir)/src

#testFont
testFont_SOURCES    = \
                       testFont.cpp

testFont_LDADD      = \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb \
                       $(TEST_LIBS)

testFont_LDFLAGS    = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L$(prefix)/lib/ginga \
                       -L$(top_srcdir)/src

#testInput
testInput_SOURCES    = \
                       testInput.cpp

testInput_LDADD      = \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb \
                       $(TEST_LIBS)

testInput_LDFLAGS    = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L$(prefix)/lib/ginga \
                       -L$(top_srcdir)/src

#testScreen
testScreen_SOURCES   = \
                       testScreen.cpp

testScreen_LDADD     = \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb \
                       $(TEST_LIBS)

testScreen_LDFLAGS   = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L$(prefix)/lib/ginga \
                       -L$(top_srcdir)/src

#testVideo
testVideo_SOURCES    = \
                       testVideo.cpp

testVideo_LDADD      = \
                       -lpthread \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb \
                       $(TEST_LIBS)

testVideo_LDFLAGS    = \
                       -no-undefined \
                       -version-info 13:1:8 \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L$(prefix)/lib/ \
                       -L$(prefix)/lib/ginga \
                       -L$(top_srcdir)/src

MAINTAINERCLEANFILES = \
                       Makefile.in
