include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
 $(INCLUDE_TELEMIDIA_UTIL)\
 $(INCLUDE_NCL30)\
 $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LIBS)

ncl30lib_LTLIBRARIES = libncl30.la
libncl30_la_SOURCES =\
  src/Base.cpp\
  src/Entity.cpp\
  src/NclDocument.cpp\
  src/Parameter.cpp\
  src/animation/Animation.cpp\
  src/components/AbsoluteReferenceContent.cpp\
  src/components/CompositeNode.cpp\
  src/components/Content.cpp\
  src/components/ContentNode.cpp\
  src/components/ContentTypeManager.cpp\
  src/components/ContextNode.cpp\
  src/components/Node.cpp\
  src/components/NodeEntity.cpp\
  src/components/PrivateBase.cpp\
  src/components/ReferenceContent.cpp\
  src/components/RelativeReferenceContent.cpp\
  src/connectors/Action.cpp\
  src/connectors/AssessmentStatement.cpp\
  src/connectors/AttributeAssessment.cpp\
  src/connectors/CausalConnector.cpp\
  src/connectors/CompoundAction.cpp\
  src/connectors/CompoundCondition.cpp\
  src/connectors/CompoundStatement.cpp\
  src/connectors/Connector.cpp\
  src/connectors/ConnectorBase.cpp\
  src/connectors/EventTransitionAssessment.cpp\
  src/connectors/EventUtil.cpp\
  src/connectors/Role.cpp\
  src/connectors/SimpleAction.cpp\
  src/connectors/SimpleCondition.cpp\
  src/connectors/TriggerExpression.cpp\
  src/connectors/ValueAssessment.cpp\
  src/descriptor/Descriptor.cpp\
  src/descriptor/DescriptorBase.cpp\
  src/interfaces/Anchor.cpp\
  src/interfaces/CircleSpatialAnchor.cpp\
  src/interfaces/ContentAnchor.cpp\
  src/interfaces/IntervalAnchor.cpp\
  src/interfaces/LabeledAnchor.cpp\
  src/interfaces/LambdaAnchor.cpp\
  src/interfaces/Port.cpp\
  src/interfaces/PropertyAnchor.cpp\
  src/interfaces/RectangleSpatialAnchor.cpp\
  src/interfaces/RelativeTimeIntervalAnchor.cpp\
  src/interfaces/SampleIntervalAnchor.cpp\
  src/interfaces/SwitchPort.cpp\
  src/interfaces/TextAnchor.cpp\
  src/layout/DeviceLayout.cpp\
  src/layout/LayoutRegion.cpp\
  src/layout/RegionBase.cpp\
  src/link/Bind.cpp\
  src/link/CausalLink.cpp\
  src/link/Link.cpp\
  src/metainformation/Meta.cpp\
  src/metainformation/Metadata.cpp\
  src/navigation/FocusDecoration.cpp\
  src/navigation/KeyNavigation.cpp\
  src/reuse/ReferNode.cpp\
  src/reuse/ReferredNode.cpp\
  src/switches/CompositeRule.cpp\
  src/switches/DescriptorSwitch.cpp\
  src/switches/Rule.cpp\
  src/switches/RuleBase.cpp\
  src/switches/SimpleRule.cpp\
  src/switches/SwitchContent.cpp\
  src/switches/SwitchNode.cpp\
  src/time/flexibility/LinearTimeCostFunction.cpp\
  src/time/flexibility/TemporalFlexibilityFunction.cpp\
  src/transition/Transition.cpp\
  src/transition/TransitionBase.cpp\
  src/transition/TransitionUtil.cpp\
  src/util/Comparator.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
ncl30include_HEADERS =\
  include/ncl/Base.h\
  include/ncl/Entity.h\
  include/ncl/IllegalBaseTypeException.h\
  include/ncl/NclDocument.h\
  include/ncl/Parameter.h\
  include/ncl/IPrivateBaseContext.h\
  $(NULL)

animationincludedir = $(ncl30includedir)/animation
animationinclude_HEADERS =\
  include/ncl/animation/Animation.h\
  $(NULL)

componentsincludedir = $(ncl30includedir)/components
componentsinclude_HEADERS =\
  include/ncl/components/AbsoluteReferenceContent.h\
  include/ncl/components/CompositeNode.h\
  include/ncl/components/Content.h\
  include/ncl/components/ContentNode.h\
  include/ncl/components/ContentTypeManager.h\
  include/ncl/components/ContextNode.h\
  include/ncl/components/IllegalNodeTypeException.h\
  include/ncl/components/Node.h\
  include/ncl/components/NodeEntity.h\
  include/ncl/components/PrivateBase.h\
  include/ncl/components/ReferenceContent.h\
  include/ncl/components/RelativeReferenceContent.h\
  $(NULL)

connectorsincludedir = $(ncl30includedir)/connectors
connectorsinclude_HEADERS =\
  include/ncl/connectors/Action.h\
  include/ncl/connectors/Assessment.h\
  include/ncl/connectors/AssessmentStatement.h\
  include/ncl/connectors/AttributeAssessment.h\
  include/ncl/connectors/CausalConnector.h\
  include/ncl/connectors/CompoundAction.h\
  include/ncl/connectors/CompoundCondition.h\
  include/ncl/connectors/CompoundStatement.h\
  include/ncl/connectors/ConditionExpression.h\
  include/ncl/connectors/Connector.h\
  include/ncl/connectors/ConnectorBase.h\
  include/ncl/connectors/EventTransitionAssessment.h\
  include/ncl/connectors/EventUtil.h\
  include/ncl/connectors/Role.h\
  include/ncl/connectors/SimpleAction.h\
  include/ncl/connectors/SimpleCondition.h\
  include/ncl/connectors/Statement.h\
  include/ncl/connectors/TriggerExpression.h\
  include/ncl/connectors/ValueAssessment.h\
  $(NULL)

descriptorincludedir = $(ncl30includedir)/descriptor
descriptorinclude_HEADERS =\
  include/ncl/descriptor/Descriptor.h\
  include/ncl/descriptor/DescriptorBase.h\
  include/ncl/descriptor/DescriptorUtil.h\
  include/ncl/descriptor/GenericDescriptor.h\
  $(NULL)

interfacesincludedir = $(ncl30includedir)/interfaces
interfacesinclude_HEADERS =\
  include/ncl/interfaces/Anchor.h\
  include/ncl/interfaces/CircleSpatialAnchor.h\
  include/ncl/interfaces/ContentAnchor.h\
  include/ncl/interfaces/InterfacePoint.h\
  include/ncl/interfaces/IntervalAnchor.h\
  include/ncl/interfaces/LabeledAnchor.h\
  include/ncl/interfaces/LambdaAnchor.h\
  include/ncl/interfaces/Port.h\
  include/ncl/interfaces/PropertyAnchor.h\
  include/ncl/interfaces/RectangleSpatialAnchor.h\
  include/ncl/interfaces/RelativeTimeIntervalAnchor.h\
  include/ncl/interfaces/SampleIntervalAnchor.h\
  include/ncl/interfaces/SpatialAnchor.h\
  include/ncl/interfaces/SwitchPort.h\
  include/ncl/interfaces/TextAnchor.h\
  $(NULL)

layoutincludedir = $(ncl30includedir)/layout
layoutinclude_HEADERS =\
  include/ncl/layout/DeviceLayout.h\
  include/ncl/layout/IDeviceLayout.h\
  include/ncl/layout/LayoutRegion.h\
  include/ncl/layout/RegionBase.h\
  $(NULL)

linkincludedir = $(ncl30includedir)/link
linkinclude_HEADERS =\
  include/ncl/link/Bind.h\
  include/ncl/link/CausalLink.h\
  include/ncl/link/Link.h\
  include/ncl/link/LinkComposition.h\
  $(NULL)

metainformationincludedir = $(ncl30includedir)/metainformation
metainformationinclude_HEADERS =\
  include/ncl/metainformation/Meta.h\
  include/ncl/metainformation/Metadata.h\
  $(NULL)

navigationincludedir = $(ncl30includedir)/navigation
navigationinclude_HEADERS =\
  include/ncl/navigation/FocusDecoration.h\
  include/ncl/navigation/KeyNavigation.h\
  $(NULL)

reuseincludedir = $(ncl30includedir)/reuse
reuseinclude_HEADERS =\
  include/ncl/reuse/ReferNode.h\
  include/ncl/reuse/ReferredNode.h\
  $(NULL)

switchesincludedir = $(ncl30includedir)/switches
switchesinclude_HEADERS = \
  include/ncl/switches/CompositeRule.h\
  include/ncl/switches/DescriptorSwitch.h\
  include/ncl/switches/Rule.h\
  include/ncl/switches/RuleBase.h\
  include/ncl/switches/SimpleRule.h\
  include/ncl/switches/SwitchContent.h\
  include/ncl/switches/SwitchNode.h\
  $(NULL)

timeincludedir = $(ncl30includedir)/time/flexibility
timeinclude_HEADERS =\
  include/ncl/time/flexibility/LinearTimeCostFunction.h\
  include/ncl/time/flexibility/TemporalFlexibilityFunction.h\
  $(NULL)

transitionincludedir = $(ncl30includedir)/transition
transitioninclude_HEADERS = \
  include/ncl/transition/Transition.h\
  include/ncl/transition/TransitionBase.h\
  include/ncl/transition/TransitionUtil.h\
  $(NULL)

utilincludedir = $(ncl30includedir)/util
utilinclude_HEADERS =\
  include/ncl/util/Comparator.h\
  $(NULL)
endif
