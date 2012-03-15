
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

#ChannelPlayer
libdir = $(prefix)/lib/ginga/players

lib_LTLIBRARIES = libgingaccplacha.la

libgingaccplacha_la_SOURCES = ChannelPlayer.cpp

libgingaccplacha_la_LIBADD = -lgingaccplayer -ltelemidiautil \
 $(COMPONENTS_LIBS) -L$(prefix)/lib -L$(prefix)/lib/ginga -L../ -L../video/

libgingaccplacha_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

#ProgramAV
lib_LTLIBRARIES += libgingaccplapav.la

libgingaccplapav_la_SOURCES = ProgramAV.cpp

libgingaccplapav_la_LIBADD = -lgingaccplayer -lgingaccplaavp \
    -ltelemidiautil $(COMPONENTS_LIBS) \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../video -L../

libgingaccplapav_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
