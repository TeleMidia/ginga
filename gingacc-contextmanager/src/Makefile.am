
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
    -I/usr/include/libuser/ $(all_includes)

LIBS =

libgingacccontextmanagerdir = /usr/local/lib/ginga

# AM_CXXFLAGS = -g

libgingacccontextmanager_so_LDFLAGS = -Wl,-soname,libgingacccontextmanager.so \
    -shared

libgingacccontextmanager_PROGRAMS = libgingacccontextmanager.so

libgingacccontextmanager_so_SOURCES = ContextManager.cpp user/GingaUser.cpp \
    system/SystemInfo.cpp

libgingacccontextmanager_so_LDADD = $(COMPONENTS_LIBS)
