
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ $(all_includes)

LIBS =

playersdir = /usr/local/etc/ginga/tools/loaders
players_PROGRAMS = players
players_SOURCES = PlayerLoader.cpp
players_LDADD = -ltelemidiautil  -lgingaccsystemprocess -lpthread -lrt \
    $(COMPONENTS_LIBS)
