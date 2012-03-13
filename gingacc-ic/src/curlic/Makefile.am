INCLUDES                      = \
                                -I$(prefix)/include/ginga/ \
                                $(all_includes) \
                                -I$(top_srcdir)/include

LIBS                          =

libdir                        = \
                                $(prefix)/lib/ginga/ic

if CURLLIB
lib_LTLIBRARIES               = \
                                libgingacciccurlic.la

libgingacciccurlic_la_SOURCES = \
                                CurlInteractiveChannel.cpp

libgingacciccurlic_la_LIBADD  = \
                                -lcurl \
                                -ltelemidiautil \
                                -lgingaccsystem \
                                -lpthread \
                                $(COMPONENTS_LIBS)
endif

MAINTAINERCLEANFILES          = \
                                Makefile.in
