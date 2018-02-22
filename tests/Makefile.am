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

AM_CPPFLAGS= -DABS_SRCDIR=\"$(abs_srcdir)\"\
  -DABS_TOP_SRCDIR=\"$(abs_top_srcdir)\"\
  -DTOP_SRCDIR=\"$(top_srcdir)\"\
  -DTOP_BUILDDIR=\"$(top_builddir)\"
AM_CXXFLAGS= $(GINGA_ALL_CXXFLAGS)
AM_LDFLAGS= -static $(GINGA_ALL_LDFLAGS)
LDADD= $(top_builddir)/lib/libginga.la
EXTRA_DIST+= tests.h

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

progs+= test-aux-ginga-parse-table
test_aux_ginga_parse_table_SOURCES= test-aux-ginga-parse-table.cpp

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

progs+= test-aux-xpathfromuri
test_aux_xpathfromuri_SOURCES= test-aux-xpathfromuri.cpp

progs+= test-aux-xstrtod
test_aux_xstrtod_SOURCES= test-aux-xstrtod.cpp

progs+= test-aux-xstrtoll
test_aux_xstrtoll_SOURCES= test-aux-xstrtoll.cpp

progs+= test-aux-xstrassign
test_aux_xstrassign_SOURCES= test-aux-xstrassign.cpp

progs+= test-aux-xstrbuild
test_aux_xstrbuild_SOURCES= test-aux-xstrbuild.cpp

progs+= test-aux-xurigetcontents
test_aux_xurigetcontents_SOURCES= test-aux-xurigetcontents.cpp

progs+= test-aux-xurifromsrc
test_aux_xurifromsrc_SOURCES= test-aux-xurifromsrc.cpp

# lib/Document.h -----------------------------------------------------------
progs+= test-Document-new
test_Document_new_SOURCES= test-Document-new.cpp

progs+= test-Document-addObject
test_Document_addObject_SOURCES= test-Document-addObject.cpp

progs+= xfail-test-Document-addObject
xfail_test_Document_addObject_SOURCES= xfail-test-Document-addObject.cpp

progs+= test-Document-getObjectById
test_Document_getObjectById_SOURCES= test-Document-getObjectById.cpp

progs+= test-Document-getObjectByIdOrAlias
test_Document_getObjectByIdOrAlias_SOURCES=\
  test-Document-getObjectByIdOrAlias.cpp

# lib/Predicate.h ----------------------------------------------------------
progs+= test-Predicate-new
test_Predicate_new_SOURCES= test-Predicate-new.cpp

progs+= test-Predicate-atom
test_Predicate_atom_SOURCES= test-Predicate-atom.cpp

# lib/Event.h --------------------------------------------------------------
progs+= test-Event-new
test_Event_new_SOURCES= test-Event-new.cpp

progs+= test-Event-isLambda
test_Event_isLambda_SOURCES= test-Event-isLambda.cpp

progs+= test-Event-setInterval
test_Event_setInterval_SOURCES= test-Event-setInterval.cpp

progs+= test-Event-setParameter
test_Event_setParameter_SOURCES= test-Event-setParameter.cpp

progs+= test-Event-getEventTypeAsString
test_Event_getEventTypeAsString_SOURCES= test-Event-getEventTypeAsString.cpp

progs+= test-Event-getEventStateAsString
test_Event_getEventStateAsString_SOURCES=\
  test-Event-getEventStateAsString.cpp

progs+= test-Event-getEventTransitionAsString
test_Event_getEventTransitionAsString_SOURCES=\
  test-Event-getEventTransitionAsString.cpp

progs+= test-Event-transition-Media-START
test_Event_transition_Media_START_SOURCES=\
  test-Event-transition-Media-START.cpp

progs+= test-Event-transition-Context-START
test_Event_transition_Context_START_SOURCES=\
  test-Event-transition-Context-START.cpp

progs+= test-Event-transition-Media-STOP
test_Event_transition_Media_STOP_SOURCES=\
  test-Event-transition-Media-STOP.cpp

progs+= test-Event-transition-Context-STOP
test_Event_transition_Context_STOP_SOURCES=\
  test-Event-transition-Context-STOP.cpp

progs+= test-Event-transition-Media-ABORT
test_Event_transition_Media_ABORT_SOURCES=\
  test-Event-transition-Media-ABORT.cpp

progs+= test-Event-transition-Context-ABORT
test_Event_transition_Context_ABORT_SOURCES=\
  test-Event-transition-Context-ABORT.cpp

progs+= test-Event-transition-Media-PAUSE
test_Event_transition_Media_PAUSE_SOURCES=\
  test-Event-transition-Media-PAUSE.cpp

progs+= test-Event-transition-Context-PAUSE
test_Event_transition_Context_PAUSE_SOURCES=\
  test-Event-transition-Context-PAUSE.cpp

progs+= test-Event-transition-Media-RESUME
test_Event_transition_Media_RESUME_SOURCES=\
  test-Event-transition-Media-RESUME.cpp

progs+= test-Event-transition-Context-RESUME
test_Event_transition_Context_RESUME_SOURCES=\
  test-Event-transition-Context-RESUME.cpp

# lib/Object.h -------------------------------------------------------------
progs+= test-Object-addAlias
test_Object_addAlias_SOURCES= test-Object-addAlias.cpp

progs+= test-Object-addEvent
test_Object_addEvent_SOURCES= test-Object-addEvent.cpp

# lib/Media.h --------------------------------------------------------------
progs+= test-Media-new
test_Media_new_SOURCES= test-Media-new.cpp

progs+= test-Media-explicitDur
test_Media_explicitDur_SOURCES= test-Media-explicitDur.cpp

progs+= test-Media-nested-explicitDur
test_Media_nested_explicitDur_SOURCES= test-Media-nested-explicitDur.cpp

# lib/MediaSettings.h ------------------------------------------------------
progs+= test-MediaSettings-new
test_MediaSettings_new_SOURCES= test-MediaSettings-new.cpp

# lib/Context.h ------------------------------------------------------------
progs+= test-Context-new
test_Context_new_SOURCES= test-Context-new.cpp

progs+= test-Context-naturalend
test_Context_naturalend_SOURCES= test-Context-naturalend.cpp

progs+= test-Context-nested-naturalend
test_Context_nested_naturalend_SOURCES= test-Context-nested-naturalend.cpp

# lib/Switch.h ------------------------------------------------------------
progs+= test-Switch-new
test_Switch_new_SOURCES= test-Switch-new.cpp

# lib/Parser.h -------------------------------------------------------------
progs+= test-Parser-parseFile
test_Parser_parseFile_SOURCES= test-Parser-parseFile.cpp

progs+= test-Parser-parseBuffer
test_Parser_parseBuffer_SOURCES= test-Parser-parseBuffer.cpp

# lib/ParserLua.h ----------------------------------------------------------
if WITH_LUA
progs+= test-ParserLua-parseBuffer
test_ParserLua_parseBuffer_SOURCES= test-ParserLua-parseBuffer.cpp
endif

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
  xfail-test-Document-addObject\
  xfail-test-Object-addEvent-unknown\
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
