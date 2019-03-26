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

# Setup test framework.
AM_COLOR_TESTS= always
TEST_EXTENSIONS= .lua
LUA_LOG_COMPILER= $(_VALGRIND) lua

# Auxiliary scripts.
noinst_SCRIPTS=

# Generate env.sh, env.csh, and gdb.cfg.
include $(top_srcdir)/build-aux/Makefile.am.env

# Configure valgrind and generate suppfile from suppfile.m4.
include $(top_srcdir)/build-aux/Makefile.am.valgrind

# Setup tests environment.
AM_TESTS_ENVIRONMENT=\
  export GST_DEBUG="*:WARN";\
  export G_DEBUG="fatal-criticals,resident-modules";\
  export G_MESSAGES_DEBUG="all";\
  export G_SLICE="always-malloc";\
  export MALLOC_CHECK_=1;\
  export LUA_PATH="$(top_builddir)/src/?.lua;;";\
  export LUA_CPATH="$(top_builddir)/src/.libs/?$(SOEXT);;";\
  $(NULL)

LD_PRELOAD= ginga$(SOEXT)

# Test scripts.
scripts=
scripts+= test-behavior.lua
scripts+= test-media-behavior-lambda.lua

check_SCRIPTS= $(scripts)
TESTS= $(check_SCRIPTS) $(check_PROGRAMS)
