# doc.mk -- Build documentation.
# Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia
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
# along with Ginga.  If not, see <https://www.gnu.org/licenses/>.

DOXYGEN= doxygen

.PHONY: doc html
doc: html

html-local: doc/Doxyfile
	$(AM_V_GEN) $(DOXYGEN) doc/Doxyfile

edit= sed -e 's,@PACKAGE_NAME\@,$(PACKAGE_NAME),g' \
	  -e 's,@PACKAGE_VERSION\@,$(PACKAGE_VERSION),g' \
	  -e 's,@PERL\@,$(PERL),g' \
	  -e 's,@top_builddir\@,$(top_builddir),g' \
	  -e 's,@top_srcdir\@,$(top_srcdir),g'

EXTRA_DIST+= doc/Doxyfile.in
EXTRA_DIST+= doc/arch.png
EXTRA_DIST+= doc/arch.tex
CLEANFILES+= doc/Doxyfile
doc/Doxyfile: $(top_srcdir)/doc/Doxyfile.in
	$(AM_V_GEN) $(edit) $(top_srcdir)/doc/Doxyfile.in >doc/Doxyfile

CLEANDIRS+= doc/html
