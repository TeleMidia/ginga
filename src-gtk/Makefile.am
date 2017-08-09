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

AM_CXXFLAGS= -I$(top_srcdir)/src $(GINGA_ALL_CXXFLAGS)
AM_LDFLAGS= $(GINGA_ALL_LDFLAGS)

bin_PROGRAMS= gingagui
gingagui_CXXFLAGS= $(AM_CXXFLAGS) $(GTK_CFLAGS)
gingagui_LDADD= $(top_builddir)/src/libginga.la
gingagui_LDFLAGS= $(AM_LDFLAGS) $(GTK_LIBS)
gingagui_SOURCES=\
  controller/AppController.cpp\
  controller/KeyEvent.cpp\
  ginga-gtk.cpp\
  model/CheckUpdates.cpp\
  model/SaveLoadSettings.cpp\
  view/AboutWindow.cpp\
  view/Draw.cpp\
  view/FullscreenWindow.cpp\
  view/MainWindow.cpp\
  view/SettingsWindow.cpp\
  view/TvControlWindow.cpp\
  $(NULL)
