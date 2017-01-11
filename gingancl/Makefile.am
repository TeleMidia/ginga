include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=
AM_CXXFLAGS=
AM_LDFLAGS= $(LTSTATIC)
noinst_LTLIBRARIES= gingancl.la

if BUILD_DATA_PROCESSING
AM_CPPFLAGS+=\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)
endif

gingancl_la_LIBADD=\
  ../ctxmgmt/ctxmgmt.la\
  ../ic/ic.la\
  ../ncl/ncl.la\
  ../system/system.la\
  ../tsparser/tsparser.la\
  ../tuner/tuner.la\
  ../gingacc-dataprocessing/libgingaccdataprocncledit.la\
  ../gingacc-dataprocessing/libgingaccdataprocessing.la\
  ../gingacc-dataprocessing/libgingaccdataprocncl.la\
  $(NULL)

gingancl_la_SOURCES=\
  AnimationController.cpp\
  FormatterBaseDevice.cpp\
  FormatterConverter.cpp\
  FormatterFocusManager.cpp\
  FormatterLinkConverter.cpp\
  FormatterMediator.cpp\
  FormatterMultiDevice.cpp\
  FormatterScheduler.cpp\
  PrefetchManager.cpp\
  PrivateBaseContext.cpp\
  PrivateBaseManager.cpp\
  adaptation/ContextBase.cpp\
  adaptation/PresentationContext.cpp\
  adaptation/RuleAdapter.cpp\
  adapters/AVPlayerAdapter.cpp\
  adapters/ApplicationPlayerAdapter.cpp\
  adapters/ChannelPlayerAdapter.cpp\
  adapters/FormatterPlayerAdapter.cpp\
  adapters/ImagePlayerAdapter.cpp\
  adapters/LuaPlayerAdapter.cpp\
  adapters/MirrorPlayerAdapter.cpp\
  adapters/NCLPlayerAdapter.cpp\
  adapters/NominalEventMonitor.cpp\
  adapters/PlainTxtPlayerAdapter.cpp\
  adapters/PlayerAdapterManager.cpp\
  adapters/ProgramAVPlayerAdapter.cpp\
  adapters/SubtitlePlayerAdapter.cpp\
  adapters/TimePlayerAdapter.cpp\
  model/AnchorEvent.cpp\
  model/ApplicationExecutionObject.cpp\
  model/AttributionEvent.cpp\
  model/BeginEventTransition.cpp\
  model/CascadingDescriptor.cpp\
  model/CompositeExecutionObject.cpp\
  model/CostFunctionDuration.cpp\
  model/DurationFactory.cpp\
  model/EndEventTransition.cpp\
  model/EventTransition.cpp\
  model/EventTransitionManager.cpp\
  model/ExecutionObject.cpp\
  model/ExecutionObjectSwitch.cpp\
  model/FlexibleTimeMeasurement.cpp\
  model/FocusSourceManager.cpp\
  model/FormatterCausalLink.cpp\
  model/FormatterDeviceRegion.cpp\
  model/FormatterEvent.cpp\
  model/FormatterLayout.cpp\
  model/FormatterLink.cpp\
  model/FormatterRegion.cpp\
  model/LinearCostFunctionDuration.cpp\
  model/LinkAction.cpp\
  model/LinkAndCompoundTriggerCondition.cpp\
  model/LinkAssessmentStatement.cpp\
  model/LinkAssignmentAction.cpp\
  model/LinkAttributeAssessment.cpp\
  model/LinkCompoundAction.cpp\
  model/LinkCompoundStatement.cpp\
  model/LinkCompoundTriggerCondition.cpp\
  model/LinkRepeatAction.cpp\
  model/LinkSimpleAction.cpp\
  model/LinkTransitionTriggerCondition.cpp\
  model/LinkTriggerCondition.cpp\
  model/LinkValueAssessment.cpp\
  model/NodeNesting.cpp\
  model/PresentationEvent.cpp\
  model/SelectionEvent.cpp\
  model/SwitchEvent.cpp\
  model/TimeMeasurement.cpp\
  model/TransitionDispatcher.cpp\
  model/UnflexibleDuration.cpp\
  $(NULL)

if BUILD_BERKELIUM
AM_CXXFLAGS+= $(BERKELIUM_CFLAGS)
AM_LDFLAGS+= $(BERKELIUM_LIBS)
gingancl_la_SOURCES+= adapters/BerkeliumPlayerAdapter.cpp
endif

if BUILD_MULTIDEVICE
gingancl_la_SOURCES+=\
  FormatterPassiveDevice.cpp\
  FormatterActiveDevice.cpp\
  $(NULL)
endif

if BUILD_SSML
gingancl_la_SOURCES+= adapters/SsmlPlayerAdapter.cpp
endif
