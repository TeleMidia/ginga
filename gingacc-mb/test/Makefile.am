
# set the include path found by configure
INCLUDES             = \
                       -I/usr/local/include/ginga/ \
                       -I$(top_srcdir)/include \
                       $(all_includes)

LIBS                 =

noinst_PROGRAMS      = \
                       testInput testScreen

#testScreen
testScreen_SOURCES   = \
                       testScreen.cpp

testScreen_LDADD     = \
                       -ldl \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb

testScreen_LDFLAGS   = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L/usr/local/lib/ \
                       -L/usr/local/lib/ginga \
                       -L$(top_srcdir)/src \
                       -L$(top_srcdir)/src/interface/dfb \
                       -L$(top_srcdir)/src/interface/term \
                       -L$(top_srcdir)/src/interface/sdl

#testInput
testInput_SOURCES    = \
                       testInput.cpp

testInput_LDADD      = \
                       -ldl \
                       -ltelemidiautil \
                       -lgingaccsystem \
                       -lgingaccmb

testInput_LDFLAGS    = \
                       -L/lib/ \
                       -L/usr/lib/ \
                       -L/usr/local/lib/ \
                       -L/usr/local/lib/ginga \
                       -L$(top_srcdir)/src \
                       -L$(top_srcdir)/src/interface/dfb \
                       -L$(top_srcdir)/src/interface/term \
                       -L$(top_srcdir)/src/interface/sdl

MAINTAINERCLEANFILES = \
                       Makefile.in
