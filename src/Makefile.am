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
include $(top_srcdir)/build-aux/Makefile.am.link
LINK_SO+= $(LTLIBRARIES:.la=$(SOEXT))

EXTRA_DIST+= ginga.pc.in
MAINTAINERCLEANFILES+= config.h.in config.h.in~
DISTCLEANFILES+= ginga.pc

pkgconfigdir= $(libdir)/pkgconfig
pkgconfig_DATA= ginga.pc

AM_CXXFLAGS= $(WARN_CFLAGS) $(WERROR_CFLAGS)
AM_LDFLAGS=

ALL_CXXFLAGS= $(AM_CXXFLAGS) $(CAIRO_CFLAGS) $(GLIB_CFLAGS)\
  $(GSTREAMER_CFLAGS) $(LIBXML_CFLAGS) $(LUA_CFLAGS) $(PANGO_CFLAGS)\
  $(GTK_CFLAGS) $(NCLUA_CFLAGS)

ALL_LDFLAGS= $(AM_LDFLAGS) $(CAIRO_LIBS) $(GLIB_LIBS)\
  $(GSTREAMER_LIBS) $(LIBXML_LIBS) $(LUA_LIBS) $(PANGO_LIBS) $(GTK_LIBS)\
  $(NCLUA_LIBS)

bin_PROGRAMS= ginga
ginga_CXXFLAGS= $(ALL_CXXFLAGS)
ginga_LDFLAGS= $(ALL_LDFLAGS) -static
ginga_SOURCES= ginga.cpp
ginga_LDADD= libginga.la

lualib_LTLIBRARIES = ginga.la
ginga_la_LIBADD = libginga.la
ginga_la_CXXFLAGS = $(ALL_CXXFLAGS)
ginga_la_LDFLAGS = $(ALL_LDFLAGS) $(LIBM) -module $(LT_MODULE_LDFLAGS)
ginga_la_SOURCES = LuaAPI_libginga.cpp

lib_LTLIBRARIES= libginga.la
libginga_la_CXXFLAGS= $(ALL_CXXFLAGS)
libginga_la_LDFLAGS= $(ALL_LDFLAGS)
libginga_la_SOURCES= $(src)
src=
src+= Behavior.cpp
src+= Composition.cpp
src+= Composition_initMt.cpp
src+= Context.cpp
src+= Context_initMt.cpp
src+= Document.cpp
src+= Document_initMt.cpp
src+= LuaAPI.cpp
src+= LuaAPI_Composition.cpp
src+= LuaAPI_Context.cpp
src+= LuaAPI_Document.cpp
src+= LuaAPI_Media.cpp
src+= LuaAPI_Object.cpp
src+= LuaAPI_Player.cpp
src+= LuaAPI_StateMachine.cpp
src+= LuaAPI_Switch.cpp
src+= Media.cpp
src+= Media_initMt.cpp
src+= Object.cpp
src+= Object_initMt.cpp
src+= Parser.cpp
src+= Player.cpp
src+= PlayerGStreamer.cpp
src+= PlayerText.cpp
src+= Player_initMt.cpp
src+= Predicate.cpp
src+= StateMachine.cpp
src+= StateMachine_initMt.cpp
src+= Switch.cpp
src+= aux-ginga.cpp
src+= initMt.cpp
src+= traceMt.cpp

if WITH_NCLUA
src+= PlayerLua.cpp
endif

# Convert .lua's to static strings.
.lua.cpp:
	@echo '#include "LuaAPI.h"' >$@
	$(AM_V_GEN)$(XXD) -i $<\
	  | sed 's,^\(unsigned [a-z]\+ \),\1LuaAPI::,' >>$@

CLEANFILES+= Behavior.cpp
CLEANFILES+= Composition_initMt.cpp
CLEANFILES+= Context_initMt.cpp
CLEANFILES+= Document_initMt.cpp
CLEANFILES+= Media_initMt.cpp
CLEANFILES+= Object_initMt.cpp
CLEANFILES+= Player_initMt.cpp
CLEANFILES+= StateMachine_initMt.cpp
CLEANFILES+= initMt.cpp
CLEANFILES+= traceMt.cpp
