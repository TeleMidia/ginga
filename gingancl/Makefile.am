include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS =\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_NCL30_CONVERTER)\
  $(INCLUDE_GINGA_NCL)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LIBS)

gingancllib_LTLIBRARIES =\
  libginganclmodel.la\
  $(NULL)

if BUILD_BERKELIUM
gingancllib_LTLIBRARIES += libginganclbklpadapter.la
endif

if BUILD_SSML
gingancllib_LTLIBRARIES += libginganclssmlpadapter.la
endif

# ginga-ncl-model
if BUILD_DATA_PROCESSING
AM_CPPFLAGS +=\
  $(INCLUDE_GINGACC_TUNER)\
  $(INCLUDE_GINGACC_TSPARSER)\
  $(INCLUDE_GINGACC_DATAPROCESSING)\
  $(NULL)
endif

libginganclmodel_la_SOURCES =\
  src/model/components/ApplicationExecutionObject.cpp\
  src/model/components/CompositeExecutionObject.cpp\
  src/model/components/ExecutionObject.cpp\
  src/model/components/NodeNesting.cpp\
  src/model/event/AnchorEvent.cpp\
  src/model/event/AttributionEvent.cpp\
  src/model/event/FormatterEvent.cpp\
  src/model/event/PresentationEvent.cpp\
  src/model/event/SelectionEvent.cpp\
  src/model/event/transition/BeginEventTransition.cpp\
  src/model/event/transition/EndEventTransition.cpp\
  src/model/event/transition/EventTransition.cpp\
  src/model/event/transition/EventTransitionManager.cpp\
  src/model/event/transition/TransitionDispatcher.cpp\
  src/model/link/FormatterCausalLink.cpp\
  src/model/link/FormatterLink.cpp\
  src/model/link/LinkAction.cpp\
  src/model/link/LinkAndCompoundTriggerCondition.cpp\
  src/model/link/LinkAssessmentStatement.cpp\
  src/model/link/LinkAssignmentAction.cpp\
  src/model/link/LinkAttributeAssessment.cpp\
  src/model/link/LinkCompoundAction.cpp\
  src/model/link/LinkCompoundStatement.cpp\
  src/model/link/LinkCompoundTriggerCondition.cpp\
  src/model/link/LinkRepeatAction.cpp\
  src/model/link/LinkSimpleAction.cpp\
  src/model/link/LinkTransitionTriggerCondition.cpp\
  src/model/link/LinkTriggerCondition.cpp\
  src/model/link/LinkValueAssessment.cpp\
  src/model/presentation/CascadingDescriptor.cpp\
  src/model/presentation/FormatterDeviceRegion.cpp\
  src/model/presentation/FormatterLayout.cpp\
  src/model/presentation/FormatterRegion.cpp\
  src/model/presentation/focus/FocusSourceManager.cpp\
  src/model/switches/ExecutionObjectSwitch.cpp\
  src/model/switches/SwitchEvent.cpp\
  src/model/time/CostFunctionDuration.cpp\
  src/model/time/DurationFactory.cpp\
  src/model/time/FlexibleTimeMeasurement.cpp\
  src/model/time/LinearCostFunctionDuration.cpp\
  src/model/time/TimeMeasurement.cpp\
  src/model/time/UnflexibleDuration.cpp\
  $(NULL)

libginganclmodel_la_LIBADD =\
  ../ctxmgmt/ctxmgmt.la\
  ../ic/ic.la\
  ../ncl30/libncl30.la\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adaptation/context/ContextBase.cpp\
  src/adaptation/context/PresentationContext.cpp\
  src/adaptation/context/RuleAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/emconverter/FormatterConverter.cpp\
  src/emconverter/FormatterLinkConverter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/multidevice/FormatterMultiDevice.cpp\
  src/multidevice/FormatterBaseDevice.cpp\
  $(NULL)

if BUILD_MULTIDEVICE
libginganclmodel_la_SOURCES +=\
  src/multidevice/FormatterPassiveDevice.cpp\
  src/multidevice/FormatterActiveDevice.cpp\
  $(NULL)
endif

libginganclmodel_la_SOURCES +=\
  src/prefetch/PrefetchManager.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/mirror/MirrorPlayerAdapter.cpp\
  src/adapters/FormatterPlayerAdapter.cpp\
  src/adapters/NominalEventMonitor.cpp\
  src/adapters/PlayerAdapterManager.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/application/ApplicationPlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/application/declarative/ncl/NCLPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-bkl-p-adapter
if BUILD_BERKELIUM
libginganclbklpadapter_la_LDFLAGS =\
  $(AM_LDFLAGS)\
  $(BERKELIUM_LIBS)
libginganclbklpadapter_la_SOURCES =\
  src/adapters/application/declarative/xhtml/berkelium/BerkeliumPlayerAdapter.cpp\
  $(NULL)
endif

libginganclmodel_la_SOURCES +=\
  src/adapters/application/imperative/lua/LuaPlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/av/AVPlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/time/TimePlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/av/tv/ProgramAVPlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/av/ChannelPlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/image/ImagePlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/text/PlainTxtPlayerAdapter.cpp\
  $(NULL)

libginganclmodel_la_SOURCES +=\
  src/adapters/text/SubtitlePlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-ssml-p-adapter
if BUILD_SSML
libginganclssmlpadapter_la_SOURCES =\
  src/adapters/tts/SsmlPlayerAdapter.cpp\
  $(NULL)
endif

# ginga-ncl
# libginganclmodel_la_LIBADD +=\
#   ../gingacc-player/player.la\
#   $(NULL)

libginganclmodel_la_SOURCES +=\
  src/FormatterMediator.cpp\
  src/FormatterScheduler.cpp\
  src/focus/FormatterFocusManager.cpp\
  src/privatebase/PrivateBaseManager.cpp\
  src/privatebase/PrivateBaseContext.cpp\
  src/animation/AnimationController.cpp\
  $(NULL)
