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

AM_CPPFLAGS= -DTOP_SRCDIR=\"$(top_srcdir)\"\
  -DTOP_BUILDDIR=\"$(top_builddir)\"
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

progs+= test-Ginga-parse-bool
test_Ginga_parse_bool_SOURCES= test-Ginga-parse-bool.cpp

progs+= test-Ginga-parse-color
test_Ginga_parse_color_SOURCES= test-Ginga-parse-color.cpp

progs+= test-Ginga-parse-list
test_Ginga_parse_list_SOURCES= test-Ginga-parse-list.cpp

progs+= test-Ginga-parse-percent
test_Ginga_parse_percent_SOURCES= test-Ginga-parse-percent.cpp

progs+= test-Ginga-parse-time
test_Ginga_parse_time_SOURCES= test-Ginga-parse-time.cpp

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

progs+= test-Ginga-version
test_Ginga_version_SOURCES= test-Ginga-version.cpp

progs+= test-Ginga-create
test_Ginga_create_SOURCES= test-Ginga-create.cpp

progs+= test-Ginga-getState
test_Ginga_getState_SOURCES= test-Ginga-getState.cpp

progs+= test-Ginga-getOptions
test_Ginga_getOptions_SOURCES= test-Ginga-getOptions.cpp

progs+= test-Ginga-getOptionBool
test_Ginga_getOptionBool_SOURCES= test-Ginga-getOptionBool.cpp

progs+= test-Ginga-setOptionBool
test_Ginga_setOptionBool_SOURCES= test-Ginga-setOptionBool.cpp

progs+= xfail-test-Ginga-getOptionBool
xfail_test_Ginga_getOptionBool_SOURCES= xfail-test-Ginga-getOptionBool.cpp

progs+= xfail-test-Ginga-setOptionBool
xfail_test_Ginga_setOptionBool_SOURCES= xfail-test-Ginga-setOptionBool.cpp

progs+= test-Ginga-getOptionInt
test_Ginga_getOptionInt_SOURCES= test-Ginga-getOptionInt.cpp

progs+= test-Ginga-setOptionInt
test_Ginga_setOptionInt_SOURCES= test-Ginga-setOptionInt.cpp

progs+= xfail-test-Ginga-getOptionInt
xfail_test_Ginga_getOptionInt_SOURCES= xfail-test-Ginga-getOptionInt.cpp

progs+= xfail-test-Ginga-setOptionInt
xfail_test_Ginga_setOptionInt_SOURCES= xfail-test-Ginga-setOptionInt.cpp

progs+= test-Ginga-getOptionString
test_Ginga_getOptionString_SOURCES= test-Ginga-getOptionString.cpp

progs+= test-Ginga-setOptionString
test_Ginga_setOptionString_SOURCES= test-Ginga-setOptionString.cpp

progs+= xfail-test-Ginga-getOptionString
xfail_test_Ginga_getOptionString_SOURCES= xfail-test-Ginga-getOptionString.cpp

progs+= xfail-test-Ginga-setOptionString
xfail_test_Ginga_setOptionString_SOURCES= xfail-test-Ginga-setOptionString.cpp

progs+= test-ParserLibXML-parseFile
test_ParserLibXML_parseFile_SOURCES= test-ParserLibXML-parseFile.cpp

progs+= test-ParserLibXML-parseBuffer
test_ParserLibXML_parseBuffer_SOURCES= test-ParserLibXML-parseBuffer.cpp

progs+= test-GingaInternal-getData
test_GingaInternal_getData_SOURCES= test-GingaInternal-getData.cpp

progs+= test-GingaInternal-setData
test_GingaInternal_setData_SOURCES= test-GingaInternal-setData.cpp

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
  xfail-test-Ginga-getOptionBool\
  xfail-test-Ginga-setOptionBool\
  xfail-test-Ginga-getOptionInt\
  xfail-test-Ginga-setOptionInt\
  xfail-test-Ginga-getOptionString\
  xfail-test-Ginga-setOptionString\
  $(NULL)
