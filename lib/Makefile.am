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
MAINTAINERCLEANFILES+= config.h.in config.h.in~

EXTRA_DIST+= ginga.pc.in
DISTCLEANFILES+= ginga.pc

pkgconfigdir= $(libdir)/pkgconfig
pkgconfig_DATA= ginga.pc

AM_CXXFLAGS= $(GINGA_ALL_CXXFLAGS)
AM_LDFLAGS= $(GINGA_ALL_LDFLAGS)

include_HEADERS= ginga.h
lib_LTLIBRARIES= libginga.la
libginga_la_CXXFLAGS= $(AM_CXXFLAGS)
libginga_la_LDFLAGS= $(AM_LDFLAGS)
libginga_la_SOURCES= $(src)
src=
src+= Formatter.cpp
src+= FormatterAction.cpp
src+= FormatterComposition.cpp
src+= FormatterCondition.cpp
src+= FormatterContext.cpp
src+= FormatterEvent.cpp
src+= FormatterLink.cpp
src+= FormatterMedia.cpp
src+= FormatterMediaSettings.cpp
src+= FormatterObject.cpp
src+= FormatterPredicate.cpp
src+= FormatterSwitch.cpp
src+= Ginga.cpp
src+= aux-ginga.cpp
src+= aux-gl.cpp
src+= ncl/NclArea.cpp
src+= ncl/NclAreaLabeled.cpp
src+= ncl/NclBind.cpp
src+= ncl/NclComposition.cpp
src+= ncl/NclContext.cpp
src+= ncl/NclDocument.cpp
src+= ncl/NclEntity.cpp
src+= ncl/NclLink.cpp
src+= ncl/NclMedia.cpp
src+= ncl/NclMediaRefer.cpp
src+= ncl/NclNode.cpp
src+= ncl/NclPort.cpp
src+= ncl/NclProperty.cpp
src+= ncl/NclSwitch.cpp
src+= ncl/ParserXercesC.cpp
src+= player/Player.cpp
src+= player/PlayerAnimator.cpp
src+= player/PlayerImage.cpp
src+= player/PlayerLua.cpp
src+= player/PlayerText.cpp
src+= player/PlayerVideo.cpp

if WITH_LIBRSVG
src+= player/PlayerSvg.cpp
endif # WITH_LIBRSVG

if WITH_LIBXML
# src+= ncl/ParserLibXML.cpp
endif # WITH_LIBXML

if WITH_CEF
src+= player/PlayerHTML.cpp
cef_filesdir= $(bindir)
cef_files_DATA=\
  $(cef_dir)/Release/chrome-sandbox\
  $(cef_dir)/Release/libcef.so\
  $(cef_dir)/Release/natives_blob.bin\
  $(cef_dir)/Release/snapshot_blob.bin\
  $(cef_dir)/Resources/cef.pak\
  $(cef_dir)/Resources/cef_100_percent.pak\
  $(cef_dir)/Resources/cef_200_percent.pak\
  $(cef_dir)/Resources/cef_extensions.pak\
  $(cef_dir)/Resources/icudtl.dat\
  $(NULL)
endif # WITH_CEF
