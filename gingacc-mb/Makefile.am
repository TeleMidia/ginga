# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4
LIBS =
SUBDIRS = 	\
			include/mb \
			include/mb/interface \
			src \
			src/interface/dfb \
			src/interface/dfb/content/audio \
			src/interface/dfb/content/image \
			src/interface/dfb/content/text \
			src/interface/dfb/content/video \
			files

MAINTAINERCLEANFILES = 	aclocal.m4 \
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
