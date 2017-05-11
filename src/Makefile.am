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
MAINTAINERCLEANFILES+= config.h.in

AM_CPPFLAGS=\
  -DGINGA_DATADIR='"$(pkgdatadir)/"'\
  -DGINGA_BUTTON_DATADIR='"$(buttondir)/"'\
  -DGINGA_CONTEXTMANAGER_DATADIR='"$(contextmanagerdir)/"'\
  -DGINGA_FONT_DATADIR='"$(fontdir)"'\
  -DGINGA_FORMATTER_DATADIR='"$(formatterdir)/"'\
  -DGINGA_FORMATTERIMG_DATADIR='"$(formatterimgdir)/"'\
  -DGINGA_MB_DATADIR='"$(mbdir)/"'\
  -DGINGA_TUNER_DATADIR='"$(tunerdir)/"'\
  $(NULL)

AM_CXXFLAGS=\
  $(CEF_CFLAGS)\
  $(GLIB_CFLAGS)\
  $(LIBRSVG_CFLAGS)\
  $(NCLUA_CFLAGS)\
  $(PANGO_CFLAGS)\
  $(PTHREAD_CFLAGS)\
  $(SDL2IMAGE_CFLAGS)\
  $(SDL2_CFLAGS)\
  $(WARN_CFLAGS)\
  $(WERROR_CFLAGS)\
  $(XERCESC_CFLAGS)\
  $(NULL)

AM_LDFLAGS= -R $(libdir) -rpath $(libdir)\
  $(CEF_LIBS)\
  $(GLIB_LIBS)\
  $(LIBRSVG_LIBS)\
  $(NCLUA_LIBS)\
  $(PANGO_LIBS)\
  $(PTHREAD_LIBS)\
  $(SDL2IMAGE_LIBS)\
  $(SDL2_LIBS)\
  $(XERCESC_LIBS)\
  $(NULL)

bin_PROGRAMS= ginga
ginga_SOURCES= $(src) main.cpp
src=
src+= ctxmgmt/ContextManager.cpp
src+= ctxmgmt/GingaUser.cpp
src+= ctxmgmt/SystemInfo.cpp
src+= formatter/AdapterApplicationPlayer.cpp
src+= formatter/AdapterFormatterPlayer.cpp
src+= formatter/AdapterImagePlayer.cpp
src+= formatter/AdapterLuaPlayer.cpp
src+= formatter/AdapterNCLPlayer.cpp
src+= formatter/AdapterPlayerManager.cpp
src+= formatter/AnimationController.cpp
src+= formatter/FormatterBaseDevice.cpp
src+= formatter/FormatterConverter.cpp
src+= formatter/FormatterFocusManager.cpp
src+= formatter/FormatterLinkConverter.cpp
src+= formatter/FormatterMediator.cpp
src+= formatter/FormatterMultiDevice.cpp
src+= formatter/FormatterScheduler.cpp
src+= formatter/NclAnchorEvent.cpp
src+= formatter/NclApplicationExecutionObject.cpp
src+= formatter/NclAttributionEvent.cpp
src+= formatter/NclBeginEventTransition.cpp
src+= formatter/NclCascadingDescriptor.cpp
src+= formatter/NclCompositeExecutionObject.cpp
src+= formatter/NclEndEventTransition.cpp
src+= formatter/NclEventTransition.cpp
src+= formatter/NclEventTransitionManager.cpp
src+= formatter/NclExecutionObject.cpp
src+= formatter/NclExecutionObjectSwitch.cpp
src+= formatter/NclFocusSourceManager.cpp
src+= formatter/NclFormatterCausalLink.cpp
src+= formatter/NclFormatterDeviceRegion.cpp
src+= formatter/NclFormatterEvent.cpp
src+= formatter/NclFormatterLayout.cpp
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
src+= formatter/NclPresentationEvent.cpp
src+= formatter/NclSelectionEvent.cpp
src+= formatter/NclSwitchEvent.cpp
src+= formatter/PresentationContext.cpp
src+= formatter/PrivateBaseContext.cpp
src+= formatter/RuleAdapter.cpp
src+= lssm/CommonCoreManager.cpp
src+= lssm/PresentationEngineManager.cpp
src+= mb/Display.cpp
src+= mb/DisplayDebug.cpp
src+= mb/KeyMap.cpp
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
src+= ncl/DeviceLayout.cpp
src+= ncl/Entity.cpp
src+= ncl/EventUtil.cpp
src+= ncl/FocusDecoration.cpp
src+= ncl/IntervalAnchor.cpp
src+= ncl/KeyNavigation.cpp
src+= ncl/LabeledAnchor.cpp
src+= ncl/LambdaAnchor.cpp
src+= ncl/LayoutRegion.cpp
src+= ncl/Link.cpp
src+= ncl/Meta.cpp
src+= ncl/Metadata.cpp
src+= ncl/NclDocument.cpp
src+= ncl/Node.cpp
src+= ncl/NodeEntity.cpp
src+= ncl/Parameter.cpp
src+= ncl/Port.cpp
src+= ncl/PrivateBase.cpp
src+= ncl/PropertyAnchor.cpp
src+= ncl/RectangleSpatialAnchor.cpp
src+= ncl/ReferNode.cpp
src+= ncl/ReferenceContent.cpp
src+= ncl/ReferredNode.cpp
src+= ncl/RegionBase.cpp
src+= ncl/RelativeTimeIntervalAnchor.cpp
src+= ncl/Role.cpp
src+= ncl/Rule.cpp
src+= ncl/RuleBase.cpp
src+= ncl/SampleIntervalAnchor.cpp
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
src+= nclconv/DocumentParser.cpp
src+= nclconv/NclComponentsConverter.cpp
src+= nclconv/NclComponentsParser.cpp
src+= nclconv/NclConnectorsConverter.cpp
src+= nclconv/NclConnectorsParser.cpp
src+= nclconv/NclDocumentConverter.cpp
src+= nclconv/NclDocumentParser.cpp
src+= nclconv/NclImportConverter.cpp
src+= nclconv/NclImportParser.cpp
src+= nclconv/NclInterfacesConverter.cpp
src+= nclconv/NclInterfacesParser.cpp
src+= nclconv/NclLayoutConverter.cpp
src+= nclconv/NclLayoutParser.cpp
src+= nclconv/NclLinkingConverter.cpp
src+= nclconv/NclLinkingParser.cpp
src+= nclconv/NclMetainformationConverter.cpp
src+= nclconv/NclMetainformationParser.cpp
src+= nclconv/NclPresentationControlConverter.cpp
src+= nclconv/NclPresentationControlParser.cpp
src+= nclconv/NclPresentationSpecConverter.cpp
src+= nclconv/NclPresentationSpecificationParser.cpp
src+= nclconv/NclStructureConverter.cpp
src+= nclconv/NclStructureParser.cpp
src+= nclconv/NclTransitionConverter.cpp
src+= nclconv/NclTransitionParser.cpp
src+= player/ImagePlayer.cpp
src+= player/LuaPlayer.cpp
src+= player/Player.cpp
src+= system/Thread.cpp
src+= util/functions.cpp

if WITH_ESPEAK
AM_CXXFLAGS+= $(ESPEAK_CFLAGS)
AM_LDFLAGS+= $(ESPEAK_LIBS)
src+= formatter/AdapterSsmlPlayer.cpp
src+= player/SsmlPlayer.cpp
endif # WITH_ESPEAK

if WITH_GSTREAMER
AM_CXXFLAGS+= $(GSTREAMER_CFLAGS)
AM_LDFLAGS+= $(GSTREAMER_LIBS)
src+= formatter/AdapterVideoPlayer.cpp
src+= player/VideoPlayer.cpp
endif # WITH_GSTREAMER

if WITH_LIBRSVG
src+= formatter/AdapterSvgPlayer.cpp
src+= player/SvgPlayer.cpp
endif #WITH_LIBRSVG

if WITH_PANGO
src+= formatter/AdapterTextPlayer.cpp
src+= player/TextPlayer.cpp
endif # WITH_PANGO

# This should be remove after the
# personalized CEF packge (.deb) is created.
#
# Uncomment the lines bellow to
# build Ginga with Chromium (CEF)
# based HTML Player.
#AM_CXXFLAGS+= -I/<path>/<to>/<cef> -I/<path>/<to>/<cef>/include
#AM_LDFLAGS+= -lcef -lcef_dll_wrapper
#src+= formatter/AdapterHTMLPlayer.cpp
#src+= player/HTMLPlayer.cpp

if WITH_CEF
src+= formatter/AdapterHTMLPlayer.cpp
src+= player/HTMLPlayer.cpp
endif # WITH_CEF

if WITH_ISDBT
AM_CPPFLAGS+=\
  -DGINGA_ISDBT_PATH='"$(gingaconfdir)/tuner/isdbt.ini"'\
  -DGINGA_TUNER_PATH='"$(gingaconfdir)/tuner/tuner.ini"'\
  $(NULL)
AM_CXXFLAGS+= $(ISDBT_CFLAGS)
AM_LDFLAGS+= $(ISDBT_LIBS)
src+= lssm/DataWrapperListener.cpp
src+= lssm/StcWrapper.cpp
src+= isdbt-dataproc/DataProcessor.cpp
src+= isdbt-dataproc/DsmccBinding.cpp
src+= isdbt-dataproc/DsmccBiop.cpp
src+= isdbt-dataproc/DsmccCrc.cpp
src+= isdbt-dataproc/DsmccDownloadDataBlock.cpp
src+= isdbt-dataproc/DsmccDownloadInfoIndication.cpp
src+= isdbt-dataproc/DsmccDownloadServerInitiate.cpp
src+= isdbt-dataproc/DsmccIor.cpp
src+= isdbt-dataproc/DsmccMessageHeader.cpp
src+= isdbt-dataproc/DsmccMessageProcessor.cpp
src+= isdbt-dataproc/DsmccModule.cpp
src+= isdbt-dataproc/DsmccMpegDescriptor.cpp
src+= isdbt-dataproc/DsmccNPTEndpoint.cpp
src+= isdbt-dataproc/DsmccNPTProcessor.cpp
src+= isdbt-dataproc/DsmccNPTReference.cpp
src+= isdbt-dataproc/DsmccObject.cpp
src+= isdbt-dataproc/DsmccObjectProcessor.cpp
src+= isdbt-dataproc/DsmccSectionPayload.cpp
src+= isdbt-dataproc/DsmccServiceDomain.cpp
src+= isdbt-dataproc/DsmccStreamEvent.cpp
src+= isdbt-dataproc/DsmccStreamMode.cpp
src+= isdbt-dataproc/DsmccTimeBaseClock.cpp
src+= isdbt-dataproc/EPGProcessor.cpp
src+= isdbt-dataproc/FilterManager.cpp
src+= isdbt-dataproc/NCLDataFile.cpp
src+= isdbt-dataproc/NCLEventDescriptor.cpp
src+= isdbt-dataproc/NCLMetadata.cpp
src+= isdbt-dataproc/NCLMetadataParser.cpp
src+= isdbt-dataproc/NCLSectionProcessor.cpp
src+= isdbt-dataproc/SectionFilter.cpp
src+= isdbt-tsparser/AIT.cpp
src+= isdbt-tsparser/Application.cpp
src+= isdbt-tsparser/ApplicationDescriptor.cpp
src+= isdbt-tsparser/ApplicationLocationDescriptor.cpp
src+= isdbt-tsparser/ApplicationNameDescriptor.cpp
src+= isdbt-tsparser/AudioComponentDescriptor.cpp
src+= isdbt-tsparser/ComponentDescriptor.cpp
src+= isdbt-tsparser/ContentAvailabilityDescriptor.cpp
src+= isdbt-tsparser/ContentDescriptor.cpp
src+= isdbt-tsparser/DataContentDescriptor.cpp
src+= isdbt-tsparser/Demuxer.cpp
src+= isdbt-tsparser/DigitalCCDescriptor.cpp
src+= isdbt-tsparser/EventInfo.cpp
src+= isdbt-tsparser/ExtendedEventDescriptor.cpp
src+= isdbt-tsparser/FrontendFilter.cpp
src+= isdbt-tsparser/LocalTimeOffsetDescriptor.cpp
src+= isdbt-tsparser/LogoTransmissionDescriptor.cpp
src+= isdbt-tsparser/PSIFilter.cpp
src+= isdbt-tsparser/ParentalRatingDescriptor.cpp
src+= isdbt-tsparser/Pat.cpp
src+= isdbt-tsparser/PesFilter.cpp
src+= isdbt-tsparser/PipeFilter.cpp
src+= isdbt-tsparser/Pmt.cpp
src+= isdbt-tsparser/PrefetchDescriptor.cpp
src+= isdbt-tsparser/SeriesDescriptor.cpp
src+= isdbt-tsparser/ServiceDescriptor.cpp
src+= isdbt-tsparser/ServiceInfo.cpp
src+= isdbt-tsparser/ShortEventDescriptor.cpp
src+= isdbt-tsparser/StreamIdentifierDescriptor.cpp
src+= isdbt-tsparser/TOT.cpp
src+= isdbt-tsparser/TSAdaptationField.cpp
src+= isdbt-tsparser/TSPacket.cpp
src+= isdbt-tsparser/TransportProtocolDescriptor.cpp
src+= isdbt-tsparser/TransportSection.cpp
src+= isdbt-tuner/Channel.cpp
src+= isdbt-tuner/FileSystemProvider.cpp
src+= isdbt-tuner/MulticastProvider.cpp
src+= isdbt-tuner/NetworkInterface.cpp
src+= isdbt-tuner/NetworkProvider.cpp
src+= isdbt-tuner/Tuner.cpp
if WITH_LINUXDVB
src+= isdbt-tuner/ISDBTFrontend.cpp
src+= isdbt-tuner/ISDBTProvider.cpp
src+= isdbt-tuner/RingBuffer.cpp
endif # WITH_LINUXDVB
endif # WITH_ISDBT
