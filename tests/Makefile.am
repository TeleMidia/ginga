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
TEST_EXTENSIONS= .ncl
LOG_COMPILER= $(_VALGRIND)
NCL_LOG_COMPILER= $(_VALGRIND) $(top_builddir)/src/ginga

# Setup tests environment.
AM_TESTS_ENVIRONMENT=\
  export GST_DEBUG="*:WARN";\
  export G_DEBUG="error";\
  export G_MESSAGES_DEBUG="all";\
  export G_SLICE="always-malloc";\
  export MALLOC_CHECK_=1;\
  $(NULL)

# Test applications.
ncls=
ncls+= xfail-test-img-open.ncl
ncls+= xfail-test-svg-open.ncl
ncls+= xfail-test-txt-open.ncl
ncls+= xfail-test-lua-open.ncl
ncls+= xfail-test-aud-open.ncl
ncls+= xfail-test-vid-open.ncl
ncls+= xfail-test-syntax-ncl-bad-tag.ncl
ncls+= xfail-test-syntax-ncl-bad-child.ncl

# Test programs.
progs=

progs+= test-ginga-parse-bool
test_ginga_parse_bool_SOURCES= test-ginga-parse-bool.cpp

progs+= test-ginga-parse-color
test_ginga_parse_color_SOURCES= test-ginga-parse-color.cpp

progs+= test-ginga-parse-list
test_ginga_parse_list_SOURCES= test-ginga-parse-list.cpp

progs+= test-ginga-parse-percent
test_ginga_parse_percent_SOURCES= test-ginga-parse-percent.cpp

progs+= test-ginga-parse-time
test_ginga_parse_time_SOURCES= test-ginga-parse-time.cpp

progs+= test-xstrispercent
test_xstrispercent_SOURCES= test-xstrispercent.cpp

progs+= test-xstrtodorpercent
test_xstrtodorpercent_SOURCES= test-xstrtodorpercent.cpp

check_SCRIPTS= $(ncls)
check_PROGRAMS= $(progs)
TESTS= $(check_SCRIPTS) $(check_PROGRAMS)
XFAIL_TESTS=\
  xfail-test-img-open.ncl\
  xfail-test-svg-open.ncl\
  xfail-test-txt-open.ncl\
  xfail-test-lua-open.ncl\
  xfail-test-aud-open.ncl\
  xfail-test-vid-open.ncl\
  xfail-test-syntax-ncl-bad-tag.ncl\
  xfail-test-syntax-ncl-bad-child.ncl\
  $(NULL)
