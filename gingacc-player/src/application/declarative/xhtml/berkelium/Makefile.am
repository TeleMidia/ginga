
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ \
    -I/usr/local/include/ $(BKL_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = /usr/local/lib/ginga/players

if BERKELIUMLIB
lib_LTLIBRARIES = libgingaccplabkl.la

libgingaccplabkl_la_SOURCES = BerkeliumHandler.cpp BerkeliumPlayer.cpp

libgingaccplabkl_la_LIBADD = -llibberkelium -ltelemidiautil -lgingaccplayer \
    -lgingaccsystem -lpthread $(COMPONENTS_LIBS) $(BKL_LIBS) \
    -L/usr/local/lib -L/usr/local/lib/ginga -L../../../../
endif
MAINTAINERCLEANFILES = Makefile.in
