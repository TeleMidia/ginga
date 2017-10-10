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
MAINTAINERCLEANFILES+= config.h.in config.h.in~

AM_CXXFLAGS= $(GINGA_ALL_CXXFLAGS)
AM_LDFLAGS= $(GINGA_ALL_LDFLAGS)

noinst_LTLIBRARIES= libginga.la
libginga_la_CXXFLAGS= $(AM_CXXFLAGS)
libginga_la_LDFLAGS= -static $(AM_LDFLAGS)
libginga_la_SOURCES= $(src)
src=
src+= aux-ginga.cpp
src+= aux-gl.cpp
src+= Ginga.cpp
src+= GingaInternal.cpp
src+= formatter/Converter.cpp
src+= formatter/ExecutionObject.cpp
src+= formatter/ExecutionObjectContext.cpp
src+= formatter/ExecutionObjectSettings.cpp
src+= formatter/ExecutionObjectSwitch.cpp
src+= formatter/NclAction.cpp
src+= formatter/NclCondition.cpp
src+= formatter/NclEventTransitionManager.cpp
src+= formatter/NclEvents.cpp
src+= formatter/NclLink.cpp
src+= formatter/RuleAdapter.cpp
src+= formatter/Scheduler.cpp
src+= ncl/Action.cpp
src+= ncl/Area.cpp
src+= ncl/AreaLabeled.cpp
src+= ncl/AttributeAssessment.cpp
src+= ncl/Base.cpp
src+= ncl/Bind.cpp
src+= ncl/CompositeRule.cpp
src+= ncl/Composition.cpp
src+= ncl/Condition.cpp
src+= ncl/Connector.cpp
src+= ncl/ConnectorBase.cpp
src+= ncl/Context.cpp
src+= ncl/Entity.cpp
src+= ncl/EventUtil.cpp
src+= ncl/Link.cpp
src+= ncl/Media.cpp
src+= ncl/NclDocument.cpp
src+= ncl/Node.cpp
src+= ncl/ParserXercesC.cpp
src+= ncl/Port.cpp
src+= ncl/Predicate.cpp
src+= ncl/Property.cpp
src+= ncl/Refer.cpp
src+= ncl/Role.cpp
src+= ncl/RuleBase.cpp
src+= ncl/SimpleRule.cpp
src+= ncl/Switch.cpp
src+= ncl/SwitchPort.cpp
src+= ncl/ValueAssessment.cpp
src+= player/ImagePlayer.cpp
src+= player/LuaPlayer.cpp
src+= player/Player.cpp
src+= player/PlayerAnimator.cpp
src+= player/TextPlayer.cpp
src+= player/VideoPlayer.cpp

if WITH_LIBRSVG
src+= player/SvgPlayer.cpp
endif # WITH_LIBRSVG

if WITH_LIBXML
src+= ncl/ParserLibXML.cpp
endif # WITH_LIBXML

if WITH_CEF
src+= player/HTMLPlayer.cpp
cef_filesdir= $(bindir)
cef_files_DATA=\
  $(cef_dir)/Release/chrome-sandbox\
  $(cef_dir)/Release/libcef.so\
  $(cef_dir)/Release/natives_blob.bin\
  $(cef_dir)/Release/snapshot_blob.bin\
  $(cef_dir)/Resources/cef.pak\
  $(cef_dir)/Resources/cef_100_percent.pak\
  $(cef_dir)/Resources/cef_200_percent.pak\
  $(cef_dir)/Resources/cef_extensions.pak\
  $(cef_dir)/Resources/icudtl.dat\
  $(NULL)
endif # WITH_CEF
