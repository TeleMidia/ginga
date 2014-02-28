
# set the include path found by configure
INCLUDES =	-I$(prefix)/include/ \
		-I${prefix}/include/ginga/ \
		$(all_includes) -I$(top_srcdir)/include  

LIBS =

libdir = $(prefix)/lib/ginga/adapters

lib_LTLIBRARIES = libginganclluapadapter.la

libginganclluapadapter_la_SOURCES = LuaPlayerAdapter.cpp

libginganclluapadapter_la_LIBADD = -lncl30 -lginganclmodel -lgingancladapter \
    -lginganclapppadapter \
    -L$(prefix)/lib/ginga -L$(prefix)/lib/ginga/mb \
    -L$(prefix)/lib/ginga/mb/dec \
    -L$(prefix)/lib/ginga/players -L../../../../model -L../../../ -L../../

libginganclluapadapter_la_LDFLAGS = \
                                    -no-undefined \
                                    -version-info 13:1:8

MAINTAINERCLEANFILES = Makefile.in
