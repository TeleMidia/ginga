
# set the include path found by configure
INCLUDES                    = \
                              -I$(prefix)/include/ginga/ \
                              -I$(prefix)/include/ \
                              -I$(prefix)/include/ \
                              -I/usr/include/Awesomium/ \
                              $(all_includes) \
                              -I$(top_srcdir)/include

LIBS                        =

libdir                      = \
                              $(prefix)/lib/ginga/players

if AWESOMIUMLIB
lib_LTLIBRARIES             = \
                              libgingaccplaawe.la

libgingaccplaawe_la_SOURCES = \
                              AwesomiumHandler.cpp \
                              AwesomiumPlayer.cpp

libgingaccplaawe_la_LIBADD  = \
                              -lawesomium-1.6.3 \
                              -ltelemidiautil \
                              -lgingaccmb \
                              -lgingaccplayer \
                              -lgingaccsystem \
                              -lpthread \
                              $(COMPONENTS_LIBS) \
                              -L$(prefix)/lib \
                              -L$(prefix)/lib/ginga \
                              -L/usr/lib/awesomium-1.6.3 \
                              -L../../../../
endif

MAINTAINERCLEANFILES        = \
                              Makefile.in
