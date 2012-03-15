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

libgingacciccurlic_la_LDFLAGS = \
                                -no-undefined \
                                -version-info 13:1:8

endif

MAINTAINERCLEANFILES          = \
                                Makefile.in
