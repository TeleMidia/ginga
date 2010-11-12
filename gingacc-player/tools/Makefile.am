
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ $(all_includes) -I$(top_srcdir)/include

LIBS =

if MULTIPROCESS
playersdir = /usr/local/etc/ginga/tools/loaders
players_PROGRAMS = players
players_SOURCES = PlayerLoader.cpp
players_LDADD = -ltelemidiautil  $(MULTIPROCESS_LIBS) $(COMPONENTS_LIBS)
endif

MAINTAINERCLEANFILES = Makefile.in
