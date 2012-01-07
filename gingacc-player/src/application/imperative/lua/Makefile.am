
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include \
    -I/usr/local/include/directfb/ -I/usr/local/include/ginga/

LIBS = 

localLibdir = /usr/local/lib/ginga/players/

libgingaccplalua_so_LDFLAGS = -Wl,-E,-soname,libgingaccplalua.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga \
	-L../../../ -L../../../image/ -L/usr/local/lib/lua/5.1 \
	-L/usr/local/lib/lua/5.1/socket

localLib_PROGRAMS = libgingaccplalua.so

libgingaccplalua_so_SOURCES = LuaEvent.cpp LuaCanvas.cpp LuaPlayer.cpp

libgingaccplalua_so_LDADD = -llua -lcore -lpthread -ltelemidiautil \
     -lgingaccsystemthread -lgingaccsystemiocodemap -lgingaccplayer \
     -lgingaccplaimg $(COMPONENTS_LIBS) $(DATAPROC_LIBS)

MAINTAINERCLEANFILES = Makefile.in
