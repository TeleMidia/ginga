# Makefile.am -- Template for generating Makefile via Automake.
#
# Copyright (C) 2006-2012 PUC-Rio/Laboratorio TeleMidia
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc., 51
# Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

SUBDIRS= tests

libdir= $(prefix)/lib/ginga/players
lib_LTLIBRARIES= libgingaccplalua.la
noinst_LTLIBRARIES= libnclua.la

# libnclua:

libnclua_la_CFLAGS= $(CFLAG_VISIBILITY) -Wall -Wextra -Werror -ansi -pedantic

libnclua_la_CPPFLAGS= -DLIBNCLUA_BUILDING $(LUA_INCLUDE)

libnclua_la_LDFLAGS= -static

libnclua_la_SOURCES=       \
  nclua-compiler-private.h \
  nclua-event.c            \
  nclua-luax-private.h     \
  nclua-luax.c             \
  nclua-private.h          \
  nclua-status.c           \
  nclua.c                  \
  nclua.h

# libgingaccplalua:

libgingaccplalua_la_CXXFLAGS= -Wall -Wextra

libgingaccplalua_la_CPPFLAGS= $(LUA_INCLUDE) \
  -I$(top_srcdir)/include -I$(prefix)/include/ginga/ $(LUA_INCLUDE)

libgingaccplalua_la_LDFLAGS= -no-undefined -export-dynamic \
  $(LUA_LIB) -lgingaccplayer

libgingaccplalua_la_LIBADD= libnclua.la

libgingaccplalua_la_SOURCES= \
  LuaCanvas.cpp              \
  LuaPlayer.cpp

MAINTAINERCLEANFILES= Makefile.in
