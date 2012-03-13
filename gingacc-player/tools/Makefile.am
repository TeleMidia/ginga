
# set the include path found by configure
INCLUDES = -I$(prefix)/include/ginga/ $(all_includes) -I$(top_srcdir)/include

LIBS =

bindir = $(prefix)/etc/ginga/tools/loaders

if MULTIPROCESS
bin_PROGRAMS    = players
players_SOURCES = PlayerLoader.cpp
players_LDADD   = -ltelemidiautil  $(MULTIPROCESS_LIBS) $(COMPONENTS_LIBS)
endif

MAINTAINERCLEANFILES = Makefile.in
