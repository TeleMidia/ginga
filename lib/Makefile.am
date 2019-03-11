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

AM_CXXFLAGS= $(GINGA_ALL_CXXFLAGS)
AM_LDFLAGS= $(GINGA_ALL_LDFLAGS)

lualib_LTLIBRARIES = ginga.la
ginga_la_LIBADD = libginga.la
ginga_la_CXXFLAGS = $(AM_CXXFLAGS) $(LUA_CFLAGS)
ginga_la_LDFLAGS = $(LIBM) -module $(LT_MODULE_LDFLAGS) $(LUA_LIBS)
ginga_la_SOURCES = LuaAPI_libginga.cpp

include_HEADERS= ginga.h
lib_LTLIBRARIES= libginga.la
libginga_la_CXXFLAGS= $(AM_CXXFLAGS)
libginga_la_LDFLAGS= $(AM_LDFLAGS)
libginga_la_SOURCES= $(src)
src=
src+= Composition.cpp
src+= Context.cpp
src+= Document.cpp
src+= Event.cpp
src+= Formatter.cpp
src+= Ginga.cpp
src+= LuaAPI.cpp
src+= LuaAPI_Composition.cpp
src+= LuaAPI_Composition_initMt.cpp
src+= LuaAPI_Context.cpp
src+= LuaAPI_Context_initMt.cpp
src+= LuaAPI_Document.cpp
src+= LuaAPI_Document_initMt.cpp
src+= LuaAPI_Event.cpp
src+= LuaAPI_Event_initMt.cpp
src+= LuaAPI_Media.cpp
src+= LuaAPI_Media_initMt.cpp
src+= LuaAPI_Object.cpp
src+= LuaAPI_Object_initMt.cpp
src+= LuaAPI_Switch.cpp
src+= LuaAPI_initMt.cpp
src+= LuaAPI_traceMt.cpp
src+= Media.cpp
src+= MediaSettings.cpp
src+= Object.cpp
src+= Parser.cpp
src+= Player.cpp
src+= PlayerAnimator.cpp
src+= PlayerImage.cpp
src+= PlayerSigGen.cpp
src+= PlayerText.cpp
src+= PlayerVideo.cpp
src+= Predicate.cpp
src+= Switch.cpp
src+= aux-ginga.cpp
src+= aux-gl.cpp

if WITH_LIBRSVG
src+= PlayerSvg.cpp
endif # WITH_LIBRSVG

if WITH_NCLUA
src+= PlayerLua.cpp
endif # WITH_NCLUA

# Convert .lua's to static strings.
.lua.cpp:
	@echo '#include "LuaAPI.h"' >$@
	$(AM_V_GEN)$(XXD) -i $< | sed s/LuaAPI_/LuaAPI::/ >>$@

CLEANFILES+= LuaAPI_Composition_initMt.cpp
CLEANFILES+= LuaAPI_Context_initMt.cpp
CLEANFILES+= LuaAPI_Document_initMt.cpp
CLEANFILES+= LuaAPI_Event_initMt.cpp
CLEANFILES+= LuaAPI_Media_initMt.cpp
CLEANFILES+= LuaAPI_Object_initMt.cpp
CLEANFILES+= LuaAPI_initMt.cpp
CLEANFILES+= LuaAPI_traceMt.cpp
