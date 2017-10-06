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

# lib/aux-ginga.h ----------------------------------------------------------
progs+= xfail-test-aux-xstrtod
xfail_test_aux_xstrtod_SOURCES= xfail-test-aux-xstrtod.cpp

progs+= test-aux-ginga-parse-bool
test_aux_ginga_parse_bool_SOURCES= test-aux-ginga-parse-bool.cpp

progs+= test-aux-ginga-parse-color
test_aux_ginga_parse_color_SOURCES= test-aux-ginga-parse-color.cpp

progs+= test-aux-ginga-parse-list
test_aux_ginga_parse_list_SOURCES= test-aux-ginga-parse-list.cpp

progs+= test-aux-ginga-parse-percent
test_aux_ginga_parse_percent_SOURCES= test-aux-ginga-parse-percent.cpp

progs+= test-aux-ginga-parse-time
test_aux_ginga_parse_time_SOURCES= test-aux-ginga-parse-time.cpp

progs+= test-aux-xstrispercent
test_aux_xstrispercent_SOURCES= test-aux-xstrispercent.cpp

progs+= test-aux-xstrtodorpercent
test_aux_xstrtodorpercent_SOURCES= test-aux-xstrtodorpercent.cpp

progs+= test-aux-xstrcasecmp
test_aux_xstrcasecmp_SOURCES= test-aux-xstrcasecmp.cpp

progs+= test-aux-xstrcaseeq
test_aux_xstrcaseeq_SOURCES= test-aux-xstrcaseeq.cpp

progs+= test-aux-xstrhasprefix
test_aux_xstrhasprefix_SOURCES= test-aux-xstrhasprefix.cpp

progs+= test-aux-xstrhassuffix
test_aux_xstrhassuffix_SOURCES= test-aux-xstrhassuffix.cpp

progs+= test-aux-xstrup
test_aux_xstrup_SOURCES= test-aux-xstrup.cpp

progs+= test-aux-xstrdown
test_aux_xstrdown_SOURCES= test-aux-xstrdown.cpp

progs+= test-aux-xstrstrip
test_aux_xstrstrip_SOURCES= test-aux-xstrstrip.cpp

progs+= test-aux-xstrsplit
test_aux_xstrsplit_SOURCES= test-aux-xstrsplit.cpp

progs+= test-aux-xpathbasename
test_aux_xpathbasename_SOURCES= test-aux-xpathbasename.cpp

progs+= test-aux-xpathdirname
test_aux_xpathdirname_SOURCES= test-aux-xpathdirname.cpp

progs+= test-aux-xpathisabs
test_aux_xpathisabs_SOURCES= test-aux-xpathisabs.cpp

progs+= test-aux-xpathisuri
test_aux_xpathisuri_SOURCES= test-aux-xpathisuri.cpp

progs+= test-aux-xpathmakeabs
test_aux_xpathmakeabs_SOURCES= test-aux-xpathmakeabs.cpp

progs+= test-aux-xpathbuild
test_aux_xpathbuild_SOURCES= test-aux-xpathbuild.cpp

progs+= test-aux-xpathbuildabs
test_aux_xpathbuildabs_SOURCES= test-aux-xpathbuildabs.cpp

progs+= test-aux-xstrtod
test_aux_xstrtod_SOURCES= test-aux-xstrtod.cpp

progs+= test-aux-xstrtoll
test_aux_xstrtoll_SOURCES= test-aux-xstrtoll.cpp

progs+= test-aux-xstrassign
test_aux_xstrassign_SOURCES= test-aux-xstrassign.cpp

progs+= test-aux-xstrbuild
test_aux_xstrbuild_SOURCES= test-aux-xstrbuild.cpp

# lib/GingaInternal.h ------------------------------------------------------
progs+= test-GingaInternal-getData
test_GingaInternal_getData_SOURCES= test-GingaInternal-getData.cpp

progs+= test-GingaInternal-setData
test_GingaInternal_setData_SOURCES= test-GingaInternal-setData.cpp

# lib/ncl/ParserLibXML.h ---------------------------------------------------
progs+= test-ParserLibXML-parseFile
test_ParserLibXML_parseFile_SOURCES= test-ParserLibXML-parseFile.cpp

progs+= test-ParserLibXML-parseBuffer
test_ParserLibXML_parseBuffer_SOURCES= test-ParserLibXML-parseBuffer.cpp

# lib/ginga.h (Ginga Library API) ------------------------------------------
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
xfail_test_Ginga_getOptionString_SOURCES=\
  xfail-test-Ginga-getOptionString.cpp

progs+= xfail-test-Ginga-setOptionString
xfail_test_Ginga_setOptionString_SOURCES=\
  xfail-test-Ginga-setOptionString.cpp

check_SCRIPTS= $(ncls)
check_PROGRAMS= $(progs)
TESTS= $(check_SCRIPTS) $(check_PROGRAMS)
XFAIL_TESTS=\
  xfail-test-Ginga-getOptionBool\
  xfail-test-Ginga-getOptionInt\
  xfail-test-Ginga-getOptionString\
  xfail-test-Ginga-setOptionBool\
  xfail-test-Ginga-setOptionInt\
  xfail-test-Ginga-setOptionString\
  xfail-test-aud-open.ncl\
  xfail-test-aux-xstrtod\
  xfail-test-img-open.ncl\
  xfail-test-lua-open.ncl\
  xfail-test-svg-open.ncl\
  xfail-test-syntax-ncl-bad-child.ncl\
  xfail-test-syntax-ncl-bad-tag.ncl\
  xfail-test-txt-open.ncl\
  xfail-test-vid-open.ncl\
  $(NULL)
