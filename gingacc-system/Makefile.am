AUTOMAKE_OPTIONS     = \
                       foreign 1.4

LIBS                 =

SUBDIRS              = \
                       include/system/thread \
                       include/system/compat \
                       include/system/fs \
                       include/system/time \
                       include/system/process \
                       src \
                       test

MAINTAINERCLEANFILES = \
                       aclocal.m4 \
                       config.log \
                       config.guess \
                       config.h.in \
                       config.status \
                       config.sub \
                       configure \
                       depcomp \
                       install-sh \
                       libtool \
                       ltmain.sh \
                       Makefile.in \
                       missing \
                       mkinstalldirs \
                       autom4te.cache/* \
                       .deps/*

maintainer-clean-local:
	rm -rf .deps
	rm -rf autom4te.cache
