# Makefile.am -- Template for generating Makefile via Automake.
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

include $(top_srcdir)/build-aux/Makefile.am.common

SUBDIRS= icons style

AM_CPPFLAGS= -I$(top_srcdir)/lib -I$(top_builddir)/lib\
  -DGINGADATADIR=\"$(gingadatadir)\"
AM_CXXFLAGS= $(CAIRO_CFLAGS) $(GLIB_CFLAGS) $(GTK_CFLAGS) $(LIBXML_CFLAGS) $(SOUP_CFLAGS)
AM_LDFLAGS= $(CAIRO_LIBS) $(GLIB_LIBS) $(GTK_LIBS) $(LIBXML_LIBS) $(SOUP_LIBS)

bin_PROGRAMS= gingagui
gingagui_CXXFLAGS= $(AM_CXXFLAGS)
gingagui_LDADD= $(top_builddir)/lib/libginga.la
gingagui_LDFLAGS= $(AM_LDFLAGS)
gingagui_SOURCES=\
  ginga-gtk.cpp\
  model/SaveLoadSettings.cpp\
  model/HttpMessage.cpp\
  view/TrackerWindow.cpp\
  view/AboutWindow.cpp\
  view/HelpWindow.cpp\
  view/Draw.cpp\
  view/FullscreenWindow.cpp\
  view/MainWindow.cpp\
  view/BigPictureWindow.cpp\
  view/TvControlWindow.cpp\
  $(NULL)
