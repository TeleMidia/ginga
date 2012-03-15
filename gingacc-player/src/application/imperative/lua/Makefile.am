
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include \
    -I$(prefix)/include/ginga/

LIBS = 

libdir = $(prefix)/lib/ginga/players

lib_LTLIBRARIES = libgingaccplalua.la

libgingaccplalua_la_SOURCES = LuaEvent.cpp LuaCanvas.cpp LuaPlayer.cpp

libgingaccplalua_la_LIBADD = -llua -lcore -lpthread -ltelemidiautil \
     -lgingaccsystem -lgingaccmb -lgingaccplayer \
     -lgingaccplaimg $(COMPONENTS_LIBS) $(DATAPROC_LIBS) \
     -L$(prefix)/lib -L$(prefix)/lib/ginga \
	 -L../../../ -L../../../image/ -L$(prefix)/lib/lua/5.1 \
	 -L$(prefix)/lib/lua/5.1/socket

libgingaccplalua_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
