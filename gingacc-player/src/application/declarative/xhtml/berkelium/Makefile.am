
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
    -I/usr/local/include/ $(BKL_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

if BERKELIUMLIB
libgingaccplabkldir = /usr/local/lib/ginga/players/

libgingaccplabkl_so_LDFLAGS = -Wl,-soname,libgingaccplabkl.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L../../../../

libgingaccplabkl_PROGRAMS = libgingaccplabkl.so

libgingaccplabkl_so_SOURCES = BerkeliumHandler.cpp BerkeliumPlayer.cpp

libgingaccplabkl_so_LDADD = -llibberkelium -ltelemidiautil -lgingaccplayer \
    -lgingaccsystemthread -lpthread $(COMPONENTS_LIBS) $(BKL_LIBS)
endif
MAINTAINERCLEANFILES = Makefile.in
