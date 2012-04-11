# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4

LIBS =

SUBDIRS = 	files \
			include/player \
			src \
			src/image \
			src/application/declarative/smil \
			src/application/declarative/xhtml/links \
			src/application/declarative/xhtml/berkelium \
			src/application/declarative/xhtml/awesomium \
			src/application/imperative/lua \
			src/text \
			src/text/plain \
			src/text/srt \
			src/video \
			src/tv \
			src/widget \
			tools \
			test

INCLUDES = -I$(prefix)/include/ -I$(prefix)/include/ginga/

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
