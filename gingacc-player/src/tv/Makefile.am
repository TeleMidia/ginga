
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
	$(all_includes) -I$(top_srcdir)/include

LIBS =

#ChannelPlayer
libgingaccplachadir = /usr/local/lib/ginga/players/

libgingaccplacha_so_LDFLAGS = -Wl,-soname,libgingaccplacha.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../ -L../video/

libgingaccplacha_PROGRAMS = libgingaccplacha.so

libgingaccplacha_so_SOURCES = ChannelPlayer.cpp

libgingaccplacha_so_LDADD = -lgingaccplayer -ltelemidiautil $(COMPONENTS_LIBS)


#ProgramAV
libgingaccplapavdir = /usr/local/lib/ginga/players/

libgingaccplapav_so_LDFLAGS = -Wl,-soname,libgingaccplapav.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../video -L../

libgingaccplapav_PROGRAMS = libgingaccplapav.so

libgingaccplapav_so_SOURCES = ProgramAV.cpp

libgingaccplapav_so_LDADD = -lgingaccplayer -lgingaccplaavp \
    -ltelemidiautil $(COMPONENTS_LIBS)

MAINTAINERCLEANFILES = Makefile.in
