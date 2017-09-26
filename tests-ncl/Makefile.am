# Makefile.am -- Template for generating Makefile via Automake.
# Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia
#
# This file is part of Ginga (Ginga-NCL).
#
# Ginga is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# Ginga is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Ginga.  If not, see <http://www.gnu.org/licenses/>.

include $(top_srcdir)/build-aux/Makefile.am.common

BUILT_SOURCES= $(TESTS_MIME) $(TESTS)

TESTS_MIME=
TESTS_MIME+= test-mime-avi.ncl
TESTS_MIME+= test-mime-nil.ncl
TESTS_MIME+= test-mime-jpg.ncl
TESTS_MIME+= test-mime-lua.ncl
TESTS_MIME+= test-mime-mp3.ncl
TESTS_MIME+= test-mime-mp4.ncl
TESTS_MIME+= test-mime-ogg.ncl
TESTS_MIME+= test-mime-ogv.ncl
TESTS_MIME+= test-mime-png.ncl
TESTS_MIME+= test-mime-svg.ncl
TESTS_MIME+= test-mime-txt.ncl

TESTS=
# empty
TESTS+= test-media-aud-empty.ncl
TESTS+= test-media-img-empty.ncl
TESTS+= test-media-lua-empty.ncl
TESTS+= test-media-nil-empty.ncl
TESTS+= test-media-svg-empty.ncl
TESTS+= test-media-txt-empty.ncl
TESTS+= test-media-vid-empty.ncl
# no region
TESTS+= test-media-aud-no-region.ncl
TESTS+= test-media-img-no-region.ncl
TESTS+= test-media-lua-no-region.ncl
TESTS+= test-media-nil-no-region.ncl
TESTS+= test-media-svg-no-region.ncl
TESTS+= test-media-txt-no-region.ncl
TESTS+= test-media-vid-no-region.ncl
# nested region
TESTS+= test-media-aud-nested-region.ncl
TESTS+= test-media-img-nested-region.ncl
TESTS+= test-media-lua-nested-region.ncl
TESTS+= test-media-nil-nested-region.ncl
TESTS+= test-media-svg-nested-region.ncl
TESTS+= test-media-txt-nested-region.ncl
TESTS+= test-media-vid-nested-region.ncl
# no descriptor
TESTS+= test-media-aud-no-desc.ncl
TESTS+= test-media-img-no-desc.ncl
TESTS+= test-media-lua-no-desc.ncl
TESTS+= test-media-nil-no-desc.ncl
TESTS+= test-media-svg-no-desc.ncl
TESTS+= test-media-txt-no-desc.ncl
TESTS+= test-media-vid-no-desc.ncl
# property: left, right, top, width, height, location, size, and bounds.
TESTS+= test-media-aud-dimensions.ncl
TESTS+= test-media-img-dimensions.ncl
TESTS+= test-media-lua-dimensions.ncl
TESTS+= test-media-nil-dimensions.ncl
TESTS+= test-media-svg-dimensions.ncl
TESTS+= test-media-txt-dimensions.ncl
TESTS+= test-media-vid-dimensions.ncl
TESTS+= test-media-aud-4grid.ncl
TESTS+= test-media-img-4grid.ncl
TESTS+= test-media-lua-4grid.ncl
TESTS+= test-media-nil-4grid.ncl
TESTS+= test-media-svg-4grid.ncl
TESTS+= test-media-txt-4grid.ncl
TESTS+= test-media-vid-4grid.ncl
# property: explicitDur
TESTS+= test-media-aud-explicitDur.ncl
TESTS+= test-media-img-explicitDur.ncl
TESTS+= test-media-lua-explicitDur.ncl
TESTS+= test-media-nil-explicitDur.ncl
TESTS+= test-media-svg-explicitDur.ncl
TESTS+= test-media-txt-explicitDur.ncl
TESTS+= test-media-vid-explicitDur.ncl
# property: focusIndex
TESTS+= test-media-aud-focusIndex.ncl
TESTS+= test-media-img-focusIndex.ncl
TESTS+= test-media-lua-focusIndex.ncl
TESTS+= test-media-nil-focusIndex.ncl
TESTS+= test-media-svg-focusIndex.ncl
TESTS+= test-media-txt-focusIndex.ncl
TESTS+= test-media-vid-focusIndex.ncl
# property: transparency
TESTS+= test-media-aud-transparency.ncl
TESTS+= test-media-img-transparency.ncl
TESTS+= test-media-lua-transparency.ncl
TESTS+= test-media-nil-transparency.ncl
TESTS+= test-media-svg-transparency.ncl
TESTS+= test-media-txt-transparency.ncl
TESTS+= test-media-vid-transparency.ncl
# property: visible
TESTS+= test-media-aud-visible.ncl
TESTS+= test-media-img-visible.ncl
TESTS+= test-media-lua-visible.ncl
TESTS+= test-media-nil-visible.ncl
TESTS+= test-media-svg-visible.ncl
TESTS+= test-media-txt-visible.ncl
TESTS+= test-media-vid-visible.ncl
# property: visible
TESTS+= test-media-aud-zIndex.ncl
TESTS+= test-media-img-zIndex.ncl
TESTS+= test-media-lua-zIndex.ncl
TESTS+= test-media-nil-zIndex.ncl
TESTS+= test-media-svg-zIndex.ncl
TESTS+= test-media-txt-zIndex.ncl
TESTS+= test-media-vid-zIndex.ncl
# link: loop
TESTS+= test-media-aud-loop.ncl
TESTS+= test-media-img-loop.ncl
TESTS+= test-media-lua-loop.ncl
TESTS+= test-media-nil-loop.ncl
TESTS+= test-media-svg-loop.ncl
TESTS+= test-media-txt-loop.ncl
TESTS+= test-media-vid-loop.ncl
# link: stop self
TESTS+= test-media-aud-stop-self.ncl
TESTS+= test-media-img-stop-self.ncl
TESTS+= test-media-lua-stop-self.ncl
TESTS+= test-media-nil-stop-self.ncl
TESTS+= test-media-svg-stop-self.ncl
TESTS+= test-media-txt-stop-self.ncl
TESTS+= test-media-vid-stop-self.ncl
# link: animation on background color
TESTS+= test-media-aud-anim-background.ncl
TESTS+= test-media-img-anim-background.ncl
TESTS+= test-media-lua-anim-background.ncl
TESTS+= test-media-nil-anim-background.ncl
TESTS+= test-media-svg-anim-background.ncl
TESTS+= test-media-txt-anim-background.ncl
TESTS+= test-media-vid-anim-background.ncl
# link: animation on width and height
TESTS+= test-media-aud-anim-dimensions.ncl
TESTS+= test-media-img-anim-dimensions.ncl
TESTS+= test-media-lua-anim-dimensions.ncl
TESTS+= test-media-nil-anim-dimensions.ncl
TESTS+= test-media-svg-anim-dimensions.ncl
TESTS+= test-media-txt-anim-dimensions.ncl
TESTS+= test-media-vid-anim-dimensions.ncl
# link: animation on transparency
TESTS+= test-media-aud-anim-transparency.ncl
TESTS+= test-media-img-anim-transparency.ncl
TESTS+= test-media-lua-anim-transparency.ncl
TESTS+= test-media-nil-anim-transparency.ncl
TESTS+= test-media-svg-anim-transparency.ncl
TESTS+= test-media-txt-anim-transparency.ncl
TESTS+= test-media-vid-anim-transparency.ncl

CLEANFILES+= $(TESTS_MIME)
$(TESTS_MIME): test-mime*.ncl.in
	$(AM_V_GEN)\
	type=`basename $@ | sed 's,test-mime-\([a-z0-9]*\)\.ncl,\1,g'`;\
	case $$type in\
	  avi) src="samples/night.avi";;\
	  jpg) src="samples/felis.jpg";;\
	  lua) src="samples/fps.lua";;\
	  mp3) src="samples/bunny.mp3";;\
	  nil) src="";;\
	  mp4) src="samples/small.mp4";;\
	  ogg) src="samples/bunny.ogg";;\
	  ogv) src="samples/clock.ogv";;\
	  png) src="samples/gnu.png";;\
	  svg) src="samples/vector.svg";;\
	  txt) src="samples/text.txt";;\
	    *) exit 1;;\
	esac;\
	sed -e "s,@SRC@,$$src,g" "test-mime.ncl.in" >$@

CLEANFILES+= $(TESTS)
$(TESTS): test-media-*.ncl.in
	$(AM_V_GEN)\
	type=`basename $@ | sed 's,test-media-\([a-z0-9]*\).*,\1,g'`;\
	suffix=`basename $@ | sed 's,test-media-[a-z0-9]*-\(.*\)\.ncl,\1,g'`;\
	case $$type in\
	  aud) src="samples/arcade.mp3";;\
	  img) src="samples/gnu.png";;\
	  lua) src="samples/fps.lua";;\
	  nil) src="";;\
	  svg) src="samples/vector.svg";;\
	  txt) src="samples/text.txt";;\
	  vid) src="samples/clock.ogv";;\
	    *) exit 1;;\
	esac;\
	sed -e "s,@SRC@,$$src,g" "test-media-$$suffix.ncl.in" >$@
