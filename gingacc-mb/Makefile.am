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
			src/interface/term \
			src/interface/term/content/audio \
			src/interface/term/content/image \
			src/interface/term/content/text \
			src/interface/term/content/video \
			src/interface/sdl \
			src/interface/sdl/content/audio \
			src/interface/sdl/content/image \
			src/interface/sdl/content/text \
			src/interface/sdl/content/video \
			files \
			test

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
