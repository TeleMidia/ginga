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

AM_CXXFLAGS= $(GINGA_ALL_CXXFLAGS)
AM_LDFLAGS= -static $(GINGA_ALL_LDFLAGS)
LDADD= $(top_builddir)/src/libginga.la

# Auxiliary programs.
noinst_PROGRAMS=

# Auxiliary scripts.
noinst_SCRIPTS=

# Generate env.sh, env.csh, and gdb.cfg.
include $(top_srcdir)/build-aux/Makefile.am.env

# Configure valgrind and generate suppfile from suppfile.m4.
include $(top_srcdir)/build-aux/Makefile.am.valgrind

# Setup test framework.
AM_COLOR_TESTS= always
DISABLE_HARD_ERRORS= true
LOG_COMPILER= $(_VALGRIND)

# Setup tests environment.
AM_TESTS_ENVIRONMENT=\
  export GST_DEBUG="*:WARN";\
  export G_DEBUG="all";\
  export G_SLICE="always-malloc";\
  export MALLOC_CHECK_=1;\
  $(NULL)

# Test programs.
progs=
progs+= test-ginga-parse-color
test_ginga_parse_color_SOURCES= test-ginga-parse-color.cpp

progs+= test-ginga-parse-list
test_ginga_parse_list_SOURCES= test-ginga-parse-list.cpp

progs+= test-ginga-parse-percent
test_ginga_parse_percent_SOURCES= test-ginga-parse-percent.cpp

progs+= test-ginga-parse-time
test_ginga_parse_time_SOURCES= test-ginga-parse-time.cpp

TESTS= $(progs)
check_PROGRAMS= $(progs)
