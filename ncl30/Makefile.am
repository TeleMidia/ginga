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
