
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
	$(all_includes)

LIBS =

libgingaccplapltdir = /usr/local/lib/ginga/players/

libgingaccplaplt_so_LDFLAGS = -Wl,-soname,libgingaccplaplt.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../ -L../

libgingaccplaplt_PROGRAMS = libgingaccplaplt.so

libgingaccplaplt_so_SOURCES = PlainTxtPlayer.cpp

libgingaccplaplt_so_LDADD = -ltelemidiautil -lgingaccplayer \
    -lpthread -lgingaccplatxt $(COMPONENTS_LIBS)
