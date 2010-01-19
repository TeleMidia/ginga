
# set the include path found by configure
INCLUDES = -I/usr/local/include/directfb/ -I/usr/local/include/ginga/ \
$(all_includes)

LIBS =

libgingacciccurlicdir = /usr/local/lib/ginga/ic

# AM_CXXFLAGS = -g

libgingacciccurlic_so_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga -Wl,-soname,libgingacciccurlic.so -shared

libgingacciccurlic_PROGRAMS = libgingacciccurlic.so

libgingacciccurlic_so_SOURCES = CurlInteractiveChannel.cpp

libgingacciccurlic_so_LDADD = -lcurl -ltelemidiautil -lpthread \
    $(COMPONENTS_LIBS)
