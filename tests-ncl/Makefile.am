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
# link: stop self
TESTS+= test-media-aud-stop-self.ncl
TESTS+= test-media-img-stop-self.ncl
TESTS+= test-media-lua-stop-self.ncl
TESTS+= test-media-nil-stop-self.ncl
TESTS+= test-media-svg-stop-self.ncl
TESTS+= test-media-txt-stop-self.ncl
TESTS+= test-media-vid-stop-self.ncl
BUILT_SOURCES= $(TESTS)

CLEANFILES+= $(TESTS)
$(TESTS): *.in
	$(AM_V_GEN)\
	type=`basename $@ | sed 's,test-media-\([a-z]*\).*,\1,g'`;\
	suffix=`basename $@ | sed 's,test-media-[a-z]*-\(.*\)\.ncl,\1,g'`;\
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
