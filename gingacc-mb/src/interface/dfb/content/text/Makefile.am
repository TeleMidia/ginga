INCLUDES                       = \
                                 -I$(prefix)/include/ginga/ \
                                 -I$(prefix)/include/directfb/ \
                                 $(all_includes) \
                                 -I$(top_srcdir)/include

LIBS                           =

libdir                         = \
                                 $(prefix)/lib/ginga/mb/dec

if DFBSYSTEM
lib_LTLIBRARIES                = \
                                 libgingaccmbdecdfbf.la

libgingaccmbdecdfbf_la_SOURCES = \
                                 DFBFontProvider.cpp

libgingaccmbdecdfbf_la_LIBADD  = \
                                 $(DFB_LIBS) \
                                 -lgingaccsystem \
                                 -lgingaccmbdfb \
                                 -lgingaccmb \
                                 -L$(prefix)/lib \
                                 -L$(prefix)/lib/ginga \
                                 -L$(top_srcdir)/src \
                                 -L$(top_srcdir)/src/interface/dfb

libgingaccmbdecdfbf_la_LDFLAGS = \
                                 -no-undefined \
                                 -version-info 13:1:8

endif #DFBSYSTEM

MAINTAINERCLEANFILES           = \
                                 Makefile.in
