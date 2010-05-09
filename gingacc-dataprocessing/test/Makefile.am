
# set the include path found by configure
INCLUDES = -I/usr/local/include/ginga/ -I/usr/local/include/directfb/ \
    $(all_includes)

LIBS =

#noinst_PROGRAMS = dsmccExtract

#dsmccExtract_SOURCES = dsmccExtract.cpp

#dsmccExtract_LDADD = -lgingaccsystemthread -ltelemidiautil \
#	-lgingacctuner -lgingacctsparser -lgingaccdataprocessing

#dsmccExtract_LDFLAGS = -L/usr/local/lib/ginga -L../src/ -L../src/dsmcc/npt \
#    -L../src/ncl -L/usr/local/lib

noinst_PROGRAMS = test

test_SOURCES = test.cpp

test_LDADD = -lgingaccsystemthread -ltelemidiautil -lgingacctuner \
    -lgingacctsparser -lgingaccdataprocessing \
    -lgingaccdataprocncl

test_LDFLAGS = -L/usr/local/lib/ginga -L../src/ -L../src/dsmcc/npt \
    -L../src/ncl -L/usr/local/lib
