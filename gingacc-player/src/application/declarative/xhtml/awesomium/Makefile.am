
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
    -I/usr/local/include/ -I/usr/local/include/ -I/usr/include/Awesomium/ \
    $(all_includes) -I$(top_srcdir)/include

LIBS =

if AWESOMIUMLIB
libgingaccplaawedir = /usr/local/lib/ginga/players/

libgingaccplaawe_so_LDFLAGS = -Wl,-soname,libgingaccplaawe.so -shared \
	-L/usr/local/lib -L/usr/local/lib/ginga -L/usr/lib/awesomium-1.6.3 -L../../../../

libgingaccplaawe_PROGRAMS = libgingaccplaawe.so

libgingaccplaawe_so_SOURCES = AwesomiumHandler.cpp AwesomiumPlayer.cpp

libgingaccplaawe_so_LDADD = -lawesomium-1.6.3 -ltelemidiautil -lgingaccplayer \
    -lgingaccsystem -lpthread $(COMPONENTS_LIBS)
endif

MAINTAINERCLEANFILES = Makefile.in
