include $(top_srcdir)/build-aux/Makefile.am.common

LDADD=\
  ../mb/mb.la\
  ../system/system.la\
  ../lssm/lssm.la\
  ../gingancl/gingancl.la\
  ../player/player.la\
  ../nclconv/nclconv.la\
  $(NULL)

if BUILD_ISDBT
LDADD+=\
  ../tuner/tuner.la\
  ../tsparser/tsparser.la\
  ../dataprocessing/dataprocessing.la\
  $(NULL)
endif

if BUILD_MULTIDEVICE
LDADD+= ../multidevice/multidevice.la
endif

AM_CXXFLAGS= $(CURL_CFLAGS) $(FFMPEG_CFLAGS) $(PTHREAD_CFLAGS)
AM_LDFLAGS= -R $(gingalibdir) -rpath $(gingalibdir)\
 $(CURL_LIBS) $(FFMPEG_LIBS) $(PTHREAD_LIBS)

bin_PROGRAMS = ginga
ginga_SOURCES= $(src) main.cpp
src=
src+= ctxmgmt/ContextManager.cpp
src+= ctxmgmt/GingaUser.cpp
src+= ctxmgmt/SystemInfo.cpp
src+= ic/CurlInteractiveChannel.cpp
src+= ic/InteractiveChannelManager.cpp
src+= ncl/Base.cpp
src+= ncl/Entity.cpp
src+= ncl/NclDocument.cpp
src+= ncl/Parameter.cpp
src+= ncl/animation/Animation.cpp
src+= ncl/components/AbsoluteReferenceContent.cpp
src+= ncl/components/CompositeNode.cpp
src+= ncl/components/Content.cpp
src+= ncl/components/ContentNode.cpp
src+= ncl/components/ContentTypeManager.cpp
src+= ncl/components/ContextNode.cpp
src+= ncl/components/Node.cpp
src+= ncl/components/NodeEntity.cpp
src+= ncl/components/PrivateBase.cpp
src+= ncl/components/ReferenceContent.cpp
src+= ncl/components/RelativeReferenceContent.cpp
src+= ncl/connectors/Action.cpp
src+= ncl/connectors/AssessmentStatement.cpp
src+= ncl/connectors/AttributeAssessment.cpp
src+= ncl/connectors/CausalConnector.cpp
src+= ncl/connectors/CompoundAction.cpp
src+= ncl/connectors/CompoundCondition.cpp
src+= ncl/connectors/CompoundStatement.cpp
src+= ncl/connectors/Connector.cpp
src+= ncl/connectors/ConnectorBase.cpp
src+= ncl/connectors/EventTransitionAssessment.cpp
src+= ncl/connectors/EventUtil.cpp
src+= ncl/connectors/Role.cpp
src+= ncl/connectors/SimpleAction.cpp
src+= ncl/connectors/SimpleCondition.cpp
src+= ncl/connectors/TriggerExpression.cpp
src+= ncl/connectors/ValueAssessment.cpp
src+= ncl/descriptor/Descriptor.cpp
src+= ncl/descriptor/DescriptorBase.cpp
src+= ncl/interfaces/Anchor.cpp
src+= ncl/interfaces/CircleSpatialAnchor.cpp
src+= ncl/interfaces/ContentAnchor.cpp
src+= ncl/interfaces/IntervalAnchor.cpp
src+= ncl/interfaces/LabeledAnchor.cpp
src+= ncl/interfaces/LambdaAnchor.cpp
src+= ncl/interfaces/Port.cpp
src+= ncl/interfaces/PropertyAnchor.cpp
src+= ncl/interfaces/RectangleSpatialAnchor.cpp
src+= ncl/interfaces/RelativeTimeIntervalAnchor.cpp
src+= ncl/interfaces/SampleIntervalAnchor.cpp
src+= ncl/interfaces/SwitchPort.cpp
src+= ncl/interfaces/TextAnchor.cpp
src+= ncl/layout/DeviceLayout.cpp
src+= ncl/layout/LayoutRegion.cpp
src+= ncl/layout/RegionBase.cpp
src+= ncl/link/Bind.cpp
src+= ncl/link/CausalLink.cpp
src+= ncl/link/Link.cpp
src+= ncl/metainformation/Meta.cpp
src+= ncl/metainformation/Metadata.cpp
src+= ncl/navigation/FocusDecoration.cpp
src+= ncl/navigation/KeyNavigation.cpp
src+= ncl/reuse/ReferNode.cpp
src+= ncl/reuse/ReferredNode.cpp
src+= ncl/switches/CompositeRule.cpp
src+= ncl/switches/DescriptorSwitch.cpp
src+= ncl/switches/Rule.cpp
src+= ncl/switches/RuleBase.cpp
src+= ncl/switches/SimpleRule.cpp
src+= ncl/switches/SwitchContent.cpp
src+= ncl/switches/SwitchNode.cpp
src+= ncl/time/LinearTimeCostFunction.cpp
src+= ncl/time/TemporalFlexibilityFunction.cpp
src+= ncl/transition/Transition.cpp
src+= ncl/transition/TransitionBase.cpp
src+= ncl/transition/TransitionUtil.cpp
src+= ncl/util/Comparator.cpp
src+= util/Base64.cpp
src+= util/Color.cpp
src+= util/functions.cpp
