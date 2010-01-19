
# set the include path found by configure
INCLUDES = $(all_includes)
LIBS =

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD =  -ldl

test_LDFLAGS = -L/lib/ -L/usr/lib/ -L/usr/local/lib/ \
    -L../src -L../src/component -L../src/parser
