
# set the include path found by configure
INCLUDES = $(all_includes) -I$(top_srcdir)/include $(COMPONENTS_INCS) \
    -I/usr/include/Awesomium/
 
LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp
test_CFLAGS = $(top_srcdir)/include
test_LDADD =  $(COMPONENTS_LIBS) $(TEST_LIBS) -ltelemidiautil \
    -lgingaccplayer -lgingaccplaavp -lgingaccplaimg -lgingaccplalua \
    -lgingaccplatxt -lgingaccplasrt -lgingaccplaplt \
    -lgingaccplacha -lgingaccplapav -lgingaccplawdg \
    -ldirectfb -llua -lcore \
    -lgingaccsystemiocodemap -lgingaccsystemthread $(MULTIPROCESS_LIBS) \
    -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L/usr/local/lib/ginga/iocontents \
    -L/usr/local/lib/ginga -L../src/ -L../src/image/ \
    -L../src/application/declarative/xhtml/links \
    -L../src/application/declarative/xhtml/berkelium \
    -L../src/application/declarative/xhtml/awesomium \
    -L../src/application/imperative/lua \
    -L../src/text -L../src/text/srt -L../src/text/plain \
    -L/usr/local/lib/lua/5.1/socket \
    -L../src/tv -L../src/video -L../src/widget

MAINTAINERCLEANFILES = Makefile.in
