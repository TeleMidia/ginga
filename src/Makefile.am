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

bin_PROGRAMS= ginga
ginga_CXXFLAGS= $(AM_CXXFLAGS)
ginga_LDFLAGS= -static $(AM_LDFLAGS)
ginga_SOURCES= main.cpp
ginga_LDADD= $(builddir)/libginga.la

noinst_LTLIBRARIES= libginga.la
libginga_la_CXXFLAGS= $(AM_CXXFLAGS)
libginga_la_LDFLAGS= -static $(AM_LDFLAGS)
libginga_la_SOURCES= $(src)
src=
src+= common-color-table.cpp
src+= common-key-table.cpp
src+= common-mime-table.cpp
src+= common.cpp
src+= formatter/Converter.cpp
src+= formatter/ExecutionObject.cpp
src+= formatter/ExecutionObjectApplication.cpp
src+= formatter/ExecutionObjectContext.cpp
src+= formatter/ExecutionObjectSwitch.cpp
src+= formatter/FocusManager.cpp
src+= formatter/FormatterScheduler.cpp
src+= formatter/NclBeginEventTransition.cpp
src+= formatter/NclCascadingDescriptor.cpp
src+= formatter/NclEndEventTransition.cpp
src+= formatter/NclEventTransition.cpp
src+= formatter/NclEventTransitionManager.cpp
src+= formatter/NclFormatterCausalLink.cpp
src+= formatter/NclFormatterEvent.cpp
src+= formatter/NclFormatterLink.cpp
src+= formatter/NclFormatterRegion.cpp
src+= formatter/NclLinkAction.cpp
src+= formatter/NclLinkAndCompoundTriggerCondition.cpp
src+= formatter/NclLinkAssessmentStatement.cpp
src+= formatter/NclLinkAssignmentAction.cpp
src+= formatter/NclLinkAttributeAssessment.cpp
src+= formatter/NclLinkCompoundAction.cpp
src+= formatter/NclLinkCompoundStatement.cpp
src+= formatter/NclLinkCompoundTriggerCondition.cpp
src+= formatter/NclLinkRepeatAction.cpp
src+= formatter/NclLinkSimpleAction.cpp
src+= formatter/NclLinkTransitionTriggerCondition.cpp
src+= formatter/NclLinkTriggerCondition.cpp
src+= formatter/NclLinkValueAssessment.cpp
src+= formatter/NclNodeNesting.cpp
src+= formatter/Parser.cpp
src+= formatter/PlayerAdapter.cpp
src+= formatter/RuleAdapter.cpp
src+= formatter/Settings.cpp
src+= mb/Dashboard.cpp
src+= mb/Display.cpp
src+= mb/SDLWindow.cpp
src+= ncl/AbsoluteReferenceContent.cpp
src+= ncl/Action.cpp
src+= ncl/Anchor.cpp
src+= ncl/Animation.cpp
src+= ncl/AssessmentStatement.cpp
src+= ncl/AttributeAssessment.cpp
src+= ncl/Base.cpp
src+= ncl/Bind.cpp
src+= ncl/CausalConnector.cpp
src+= ncl/CausalLink.cpp
src+= ncl/Comparator.cpp
src+= ncl/CompositeNode.cpp
src+= ncl/CompositeRule.cpp
src+= ncl/CompoundAction.cpp
src+= ncl/CompoundCondition.cpp
src+= ncl/CompoundStatement.cpp
src+= ncl/Connector.cpp
src+= ncl/ConnectorBase.cpp
src+= ncl/Content.cpp
src+= ncl/ContentAnchor.cpp
src+= ncl/ContentNode.cpp
src+= ncl/ContextNode.cpp
src+= ncl/Descriptor.cpp
src+= ncl/DescriptorBase.cpp
src+= ncl/DescriptorSwitch.cpp
src+= ncl/Entity.cpp
src+= ncl/EventUtil.cpp
src+= ncl/FocusDecoration.cpp
src+= ncl/IntervalAnchor.cpp
src+= ncl/KeyNavigation.cpp
src+= ncl/LabeledAnchor.cpp
src+= ncl/LambdaAnchor.cpp
src+= ncl/LayoutRegion.cpp
src+= ncl/Link.cpp
src+= ncl/NclDocument.cpp
src+= ncl/Node.cpp
src+= ncl/NodeEntity.cpp
src+= ncl/Parameter.cpp
src+= ncl/Port.cpp
src+= ncl/PropertyAnchor.cpp
src+= ncl/RectangleSpatialAnchor.cpp
src+= ncl/ReferNode.cpp
src+= ncl/ReferenceContent.cpp
src+= ncl/ReferredNode.cpp
src+= ncl/RegionBase.cpp
src+= ncl/Role.cpp
src+= ncl/Rule.cpp
src+= ncl/RuleBase.cpp
src+= ncl/SimpleAction.cpp
src+= ncl/SimpleCondition.cpp
src+= ncl/SimpleRule.cpp
src+= ncl/SwitchContent.cpp
src+= ncl/SwitchNode.cpp
src+= ncl/SwitchPort.cpp
src+= ncl/TextAnchor.cpp
src+= ncl/Transition.cpp
src+= ncl/TransitionBase.cpp
src+= ncl/TransitionUtil.cpp
src+= ncl/TriggerExpression.cpp
src+= ncl/ValueAssessment.cpp
src+= player/ImagePlayer.cpp
src+= player/LuaPlayer.cpp
src+= player/Player.cpp
src+= player/PlayerAnimator.cpp
src+= player/VideoPlayer.cpp

if WITH_LIBRSVG
src+= player/SvgPlayer.cpp
endif # WITH_LIBRSVG

if WITH_PANGO
src+= player/TextPlayer.cpp
endif # WITH_PANGO

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
