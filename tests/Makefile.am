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
LDADD= $(top_builddir)/lib/libginga.la

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

progs+= xfail-test-xstrtod
xfail_test_xstrtod_SOURCES= xfail-test-xstrtod.cpp

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

progs+= test-xstrcasecmp
test_xstrcasecmp_SOURCES= test-xstrcasecmp.cpp

progs+= test-xstrcaseeq
test_xstrcaseeq_SOURCES= test-xstrcaseeq.cpp

progs+= test-xstrhasprefix
test_xstrhasprefix_SOURCES= test-xstrhasprefix.cpp

progs+= test-xstrhassuffix
test_xstrhassuffix_SOURCES= test-xstrhassuffix.cpp

progs+= test-xstrup
test_xstrup_SOURCES= test-xstrup.cpp

progs+= test-xstrdown
test_xstrdown_SOURCES= test-xstrdown.cpp

progs+= test-xstrstrip
test_xstrstrip_SOURCES= test-xstrstrip.cpp

progs+= test-xstrsplit
test_xstrsplit_SOURCES= test-xstrsplit.cpp

progs+= test-xpathbasename
test_xpathbasename_SOURCES= test-xpathbasename.cpp

progs+= test-xpathdirname
test_xpathdirname_SOURCES= test-xpathdirname.cpp

progs+= test-xpathisabs
test_xpathisabs_SOURCES= test-xpathisabs.cpp

progs+= test-xpathisuri
test_xpathisuri_SOURCES= test-xpathisuri.cpp

progs+= test-xpathmakeabs
test_xpathmakeabs_SOURCES= test-xpathmakeabs.cpp

progs+= test-xpathbuild
test_xpathbuild_SOURCES= test-xpathbuild.cpp

progs+= test-xpathbuildabs
test_xpathbuildabs_SOURCES= test-xpathbuildabs.cpp

progs+= test-xstrtod
test_xstrtod_SOURCES= test-xstrtod.cpp

progs+= test-xstrtoll
test_xstrtoll_SOURCES= test-xstrtoll.cpp

progs+= test-xstrassign
test_xstrassign_SOURCES= test-xstrassign.cpp

progs+= test-xstrbuild
test_xstrbuild_SOURCES= test-xstrbuild.cpp

progs+= test-ginga-version
test_ginga_version_SOURCES= test-ginga-version.cpp

progs+= test-ginga-create
test_ginga_create_SOURCES= test-ginga-create.cpp

progs+= test-ginga-getOptions
test_ginga_getOptions_SOURCES= test-ginga-getOptions.cpp

progs+= test-ginga-getOptionBool
test_ginga_getOptionBool_SOURCES= test-ginga-getOptionBool.cpp

progs+= test-ginga-setOptionBool
test_ginga_setOptionBool_SOURCES= test-ginga-setOptionBool.cpp

progs+= xfail-test-ginga-getOptionBool
xfail_test_ginga_getOptionBool_SOURCES= xfail-test-ginga-getOptionBool.cpp

progs+= xfail-test-ginga-setOptionBool
xfail_test_ginga_setOptionBool_SOURCES= xfail-test-ginga-setOptionBool.cpp

progs+= test-ginga-getOptionInt
test_ginga_getOptionInt_SOURCES= test-ginga-getOptionInt.cpp

progs+= test-ginga-setOptionInt
test_ginga_setOptionInt_SOURCES= test-ginga-setOptionInt.cpp

progs+= xfail-test-ginga-getOptionInt
xfail_test_ginga_getOptionInt_SOURCES= xfail-test-ginga-getOptionInt.cpp

progs+= xfail-test-ginga-setOptionInt
xfail_test_ginga_setOptionInt_SOURCES= xfail-test-ginga-setOptionInt.cpp

progs+= test-ginga-getOptionString
test_ginga_getOptionString_SOURCES= test-ginga-getOptionString.cpp

progs+= test-ginga-setOptionString
test_ginga_setOptionString_SOURCES= test-ginga-setOptionString.cpp

progs+= xfail-test-ginga-getOptionString
xfail_test_ginga_getOptionString_SOURCES= xfail-test-ginga-getOptionString.cpp

progs+= xfail-test-ginga-setOptionString
xfail_test_ginga_setOptionString_SOURCES= xfail-test-ginga-setOptionString.cpp

progs+= test-ParserLibXML-parseFile
test_ParserLibXML_parseFile_SOURCES= test-ParserLibXML-parseFile.cpp

progs+= test-ParserLibXML-parseBuffer
test_ParserLibXML_parseBuffer_SOURCES= test-ParserLibXML-parseBuffer.cpp

progs+= test-GingaState-getData
test_GingaState_getData_SOURCES= test-GingaState-getData.cpp

progs+= test-GingaState-setData
test_GingaState_setData_SOURCES= test-GingaState-setData.cpp

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
  xfail-test-xstrtod\
  xfail-test-ginga-getOptionBool\
  xfail-test-ginga-setOptionBool\
  xfail-test-ginga-getOptionInt\
  xfail-test-ginga-setOptionInt\
  xfail-test-ginga-getOptionString\
  xfail-test-ginga-setOptionString\
  $(NULL)
