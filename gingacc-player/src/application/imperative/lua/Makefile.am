
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include \
    -I/usr/local/include/ginga/

LIBS = 

libdir = /usr/local/lib/ginga/players

lib_LTLIBRARIES = libgingaccplalua.la

libgingaccplalua_la_SOURCES = LuaEvent.cpp LuaCanvas.cpp LuaPlayer.cpp

libgingaccplalua_la_LIBADD = -llua -lcore -lpthread -ltelemidiautil \
     -lgingaccsystem -lgingaccmb -lgingaccplayer \
     -lgingaccplaimg $(COMPONENTS_LIBS) $(DATAPROC_LIBS) \
     -L/usr/local/lib -L/usr/local/lib/ginga \
	 -L../../../ -L../../../image/ -L/usr/local/lib/lua/5.1 \
	 -L/usr/local/lib/lua/5.1/socket

MAINTAINERCLEANFILES = Makefile.in
