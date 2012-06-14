# not a GNU package. You can remove this line, if
# have all needed files, that a GNU package needs
AUTOMAKE_OPTIONS = foreign 1.4
ACLOCAL_AMFLAGS= -Im4

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
			config.h.in \
			config.h.in~ \
			config.status \
			configure \
			Makefile.in \
			m4/libtool.m4 \
			m4/ltoptions.m4 \
			m4/ltsugar.m4 \
			m4/ltversion.m4 \
			m4/lt~obsolete.m4

maintainer-clean-local:
	rm -rf build-aux
	rm -rf autom4te.cache
