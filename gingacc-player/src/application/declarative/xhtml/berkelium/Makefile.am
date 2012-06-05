INCLUDES                    = \
                              -I$(prefix)/include/ginga/ \
                              -I$(prefix)/include/ \
                              -I$(prefix)/include/ \
                              $(all_includes) \
                              -I$(top_srcdir)/include

LIBS                        =

libdir                      = \
                              $(prefix)/lib/ginga/players

if BERKELIUMLIB
lib_LTLIBRARIES             = \
                              libgingaccplabkl.la

libgingaccplabkl_la_SOURCES = \
                              BerkeliumHandler.cpp \
                              BerkeliumPlayer.cpp

libgingaccplabkl_la_LIBADD = \
                              -llibberkelium \
                              -ltelemidiautil \
                              -lgingaccmb \
                              -lgingaccplayer \
                              -lgingaccsystem \
                              -lpthread \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib \
                              -L$(prefix)/lib/ginga \
                              -L../../../../

libgingaccplabkl_la_LDFLAGS = \
                              -no-undefined \
                              -version-info 13:1:8
endif

MAINTAINERCLEANFILES        = \
                              Makefile.in
