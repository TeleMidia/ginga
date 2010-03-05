
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ $(all_includes)

LIBS =

loaderplayerdir = /usr/local/etc/ginga/tools/
loaderplayer_PROGRAMS = loaderplayer
loaderplayer_SOURCES = PlayerLoader.cpp
loaderplayer_LDADD = -ltelemidiautil  -lgingaccsystemprocess -lpthread -lrt \
    $(COMPONENTS_LIBS)
