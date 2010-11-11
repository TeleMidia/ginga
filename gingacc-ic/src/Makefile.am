
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes) -I$(top_srcdir)/include

LIBS =

libgingaccicdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libgingaccic_so_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -L./curlic -Wl,-soname,libgingaccic.so -shared

libgingaccic_PROGRAMS = libgingaccic.so

libgingaccic_so_SOURCES = InteractiveChannelManager.cpp

libgingaccic_so_LDADD = -lpthread $(ICM_COMPONENTS_LIBS)
