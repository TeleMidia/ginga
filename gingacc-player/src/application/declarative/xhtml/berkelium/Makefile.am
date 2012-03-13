
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ \
    -I$(prefix)/include/ $(BKL_CFLAGS) $(all_includes) -I$(top_srcdir)/include

LIBS =

libdir = $(prefix)/lib/ginga/players

if BERKELIUMLIB
lib_LTLIBRARIES = libgingaccplabkl.la

libgingaccplabkl_la_SOURCES = BerkeliumHandler.cpp BerkeliumPlayer.cpp

libgingaccplabkl_la_LIBADD = -llibberkelium -ltelemidiautil -lgingaccplayer \
    -lgingaccsystem -lpthread $(COMPONENTS_LIBS) $(BKL_LIBS) \
    -L$(prefix)/lib -L$(prefix)/lib/ginga -L../../../../
endif
MAINTAINERCLEANFILES = Makefile.in
