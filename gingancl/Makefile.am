include $(top_srcdir)/build-aux/Makefile.am.common

################################ Libraries #################################
AM_CPPFLAGS =\
  $(INCLUDE_TELEMIDIA_UTIL)\
  $(INCLUDE_GINGACC_SYSTEM)\
  $(INCLUDE_GINGACC_CM)\
  $(INCLUDE_GINGACC_IC)\
  $(INCLUDE_GINGACC_MB)\
  $(INCLUDE_GINGACC_CONTEXTMANAGER)\
  $(INCLUDE_GINGACC_MULTIDEVICE)\
  $(INCLUDE_GINGACC_PLAYER)\
  $(INCLUDE_NCL30)\
  $(INCLUDE_NCL30_CONVERTER)\
  $(INCLUDE_GINGA_NCL)\
  $(NULL)

AM_CXXFLAGS = $(PTHREAD_CFLAGS)
AM_LDFLAGS = $(LTLDFLAGS) $(PTHREAD_LIBS)

gingancllib_LTLIBRARIES =\
  libginganclmodel.la\
  libgingancladaptation.la\
  libginganclemconverter.la\
  libginganclmd.la\
  libginganclprefetch.la\
  libgingancladapter.la\
  libginganclapppadapter.la\
  libginganclnclpadapter.la\
  libginganclluapadapter.la\
  libginganclaavpadapter.la\
  libginganclpavpadapter.la\
  libginganclchapadapter.la\
  libginganclimgpadapter.la\
  libginganclpltpadapter.la\
  libginganclsubpadapter.la\
  libginganclmirpadapter.la\
  libgingancltimpadapter.la\
  libgingancl.la\
  $(NULL)

if BUILD_BERKELIUM
gingancllib_LTLIBRARIES += libginganclbklpadapter.la
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
  ../ncl30/libncl30.la\
  $(NULL)

# ginga-ncl-adaptation
libgingancladaptation_la_SOURCES =\
  src/adaptation/context/ContextBase.cpp\
  src/adaptation/context/PresentationContext.cpp\
  src/adaptation/context/RuleAdapter.cpp\
  $(NULL)

libgingancladaptation_la_LIBADD =\
  ../ncl30/libncl30.la\
  $(NULL)

# ginga-ncl-em-converter
libginganclemconverter_la_LIBADD =\
  libginganclmodel.la\
  $(NULL)

libginganclemconverter_la_SOURCES =\
  src/emconverter/FormatterConverter.cpp\
  src/emconverter/FormatterLinkConverter.cpp\
  $(NULL)

# ginga-ncl-md
libginganclmd_la_SOURCES =\
  src/multidevice/FormatterMultiDevice.cpp\
  src/multidevice/FormatterBaseDevice.cpp\
  $(NULL)

if BUILD_MULTIDEVICE
libginganclmd_la_SOURCES +=\
  src/multidevice/FormatterPassiveDevice.cpp\
  src/multidevice/FormatterActiveDevice.cpp\
  $(NULL)
endif

libginganclmd_la_LIBADD =\
  libgingancladaptation.la\
  libginganclmodel.la\
  $(NULL)

# ginga-ncl-prefetch
libginganclprefetch_la_SOURCES =\
  src/prefetch/PrefetchManager.cpp\
  $(NULL)

# ginga-ncl-adapter
libgingancladapter_la_LIBADD =\
  libginganclmodel.la\
  $(NULL)

libgingancladapter_la_SOURCES =\
  src/adapters/mirror/MirrorPlayerAdapter.cpp\
  src/adapters/FormatterPlayerAdapter.cpp\
  src/adapters/NominalEventMonitor.cpp\
  src/adapters/PlayerAdapterManager.cpp\
  $(NULL)

# ginga-ncl-app-adapter
libginganclapppadapter_la_SOURCES =\
  src/adapters/application/ApplicationPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-ncl-p-adapter
libginganclnclpadapter_la_LIBADD =\
  $(NULL)

libginganclnclpadapter_la_SOURCES =\
  src/adapters/application/declarative/ncl/NCLPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-bkl-p-adapter
if BUILD_BERKELIUM
libginganclbklpadapter_la_LIBADD =\
  libginganclapppadapter.la\
  $(NULL)

libginganclbklpadapter_la_LDFLAGS =\
  $(AM_LDFLAGS)\
  $(BERKELIUM_LIBS)
libginganclbklpadapter_la_SOURCES =\
  src/adapters/application/declarative/xhtml/berkelium/BerkeliumPlayerAdapter.cpp\
  $(NULL)
endif

# ginga-ncl-lua-p-adapter
libginganclluapadapter_la_LIBADD =\
  libginganclapppadapter.la\
  $(NULL)

libginganclluapadapter_la_SOURCES =\
  src/adapters/application/imperative/lua/LuaPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-av-p-adapter
libginganclaavpadapter_la_SOURCES =\
  src/adapters/av/AVPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-mir-p-adapter
libginganclmirpadapter_la_LIBADD =\
  $(NULL)

libginganclmirpadapter_la_SOURCES =\
  src/adapters/mirror/MirrorPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-tim-p-adapter
libgingancltimpadapter_la_LIBADD =\
  $(NULL)

libgingancltimpadapter_la_SOURCES =\
  src/adapters/time/TimePlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-pav-p-adapter
libginganclpavpadapter_la_SOURCES =\
  src/adapters/av/tv/ProgramAVPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-cha-p-adapter
libginganclchapadapter_la_SOURCES =\
  src/adapters/av/ChannelPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-img-p-adapter
libginganclimgpadapter_la_SOURCES =\
  src/adapters/image/ImagePlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-plt-p-adapter
libginganclpltpadapter_la_SOURCES =\
  src/adapters/text/PlainTxtPlayerAdapter.cpp\
  $(NULL)

# ginga-ncl-sub-p-adapter
libginganclsubpadapter_la_SOURCES =\
  src/adapters/text/SubtitlePlayerAdapter.cpp\
  $(NULL)

# ginga-ncl
libgingancl_la_LIBADD =\
  ../gingacc-player/libgingaccplayer.la\
  libginganclmodel.la\
  libginganclemconverter.la\
  libginganclmd.la\
  $(NULL)

libgingancl_la_SOURCES =\
  src/FormatterMediator.cpp\
  src/FormatterScheduler.cpp\
  src/focus/FormatterFocusManager.cpp\
  src/privatebase/PrivateBaseManager.cpp\
  src/privatebase/PrivateBaseContext.cpp\
  src/animation/AnimationController.cpp\
  $(NULL)

################################# Headers ##################################
if INSTALL_HEADERS
ginganclinclude_HEADERS =\
  include/gingancl/IFormatterSchedulerListener.h\
  include/gingancl/IFormatterScheduler.h\
  $(NULL)

privatebaseincludedir = $(ginganclincludedir)/privatebase
privatebaseinclude_HEADERS =\
  include/gingancl/privatebase/IPrivateBaseManager.h\
  include/gingancl/animation/AnimationController.h\
  $(NULL)

contextincludedir = $(ginganclincludedir)/adaptation/context
contextinclude_HEADERS =\
  include/gingancl/adaptation/context/IPresentationContext.h\
  $(NULL)

adaptersincludedir = $(ginganclincludedir)/adapters
adaptersinclude_HEADERS =\
  include/gingancl/adapters/FormatterPlayerAdapter.h\
  include/gingancl/adapters/NominalEventMonitor.h\
  include/gingancl/adapters/PlayerAdapterManager.h\
  include/gingancl/adapters/IPlayerAdapterManager.h\
  include/gingancl/adapters/IPlayerAdapter.h\
  $(NULL)

adaptersapplicationincludedir = $(adaptersincludedir)/application
adaptersapplicationinclude_HEADERS =\
  include/gingancl/adapters/application/ApplicationPlayerAdapter.h\
  $(NULL)

adaptersapplicationnclincludedir =\
  $(adaptersapplicationincludedir)/declarative/ncl
adaptersapplicationnclinclude_HEADERS =\
  include/gingancl/adapters/application/declarative/ncl/NCLPlayerAdapter.h\
  $(NULL)

if BUILD_BERKELIUM
adaptersapplicationberkeliumincludedir =\
  $(adaptersapplicationincludedir)/declarative/xhtml/berkelium
adaptersapplicationberkeliuminclude_HEADERS =\
  include/gingancl/adapters/application/declarative/xhtml/berkelium/BerkeliumPlayerAdapter.h\
  $(NULL)
endif

adaptersapplicationluaincludedir =\
  $(adaptersapplicationincludedir)/imperative/lua
adaptersapplicationluainclude_HEADERS =\
  include/gingancl/adapters/application/imperative/lua/LuaPlayerAdapter.h\
  $(NULL)

adaptersavincludedir = $(adaptersincludedir)/av
adaptersavinclude_HEADERS =\
  include/gingancl/adapters/av/AVPlayerAdapter.h\
  include/gingancl/adapters/av/ChannelPlayerAdapter.h\
  $(NULL)

adaptersmirincludedir = $(adaptersincludedir)/mirror
adaptersmirinclude_HEADERS =\
  include/gingancl/adapters/mirror/MirrorPlayerAdapter.h\
  $(NULL)

adapterstimincludedir = $(adaptersincludedir)/time
adapterstiminclude_HEADERS =\
  include/gingancl/adapters/time/TimePlayerAdapter.h\
  $(NULL)

adatpersavtvincludedir = $(adaptersavincludedir)/tv
adatpersavtvinclude_HEADERS =\
  include/gingancl/adapters/av/tv/ProgramAVPlayerAdapter.h\
  $(NULL)

adaptersimageincludedir = $(adaptersincludedir)/image
adaptersimageinclude_HEADERS =\
  include/gingancl/adapters/image/ImagePlayerAdapter.h\
  $(NULL)

adapterstextincludedir = $(adaptersincludedir)/text
adapterstextinclude_HEADERS =\
  include/gingancl/adpaters/text/PlainTxtPlayerAdapter.h\
  include/gingancl/adpaters/text/SubtitlePlayerAdapter.h\
  $(NULL)

emconverterincludedir = $(ginganclincludedir)/emconverter
emconverterinclude_HEADERS =\
  include/gingancl/emconverter/FormatterConverter.h\
  include/gingancl/emconverter/FormatterLinkConverter.h\
  include/gingancl/emconverter/ObjectCreationForbiddenException.h\
  $(NULL)

focusincludedir = $(ginganclincludedir)/focus
focusinclude_HEADERS =\
  include/gingancl/focus/IFormatterFocusManager.h\
  include/gingancl/focus/FormatterFocusManager.h\
  $(NULL)

modelincludedir = $(ginganclincludedir)/model
modelinclude_HEADERS =\
  include/gingancl/model/AnchorEvent.h\
  include/gingancl/model/AttributionEvent.h\
  include/gingancl/model/BeginEventTransition.h\
  include/gingancl/model/CascadingDescriptor.h\
  include/gingancl/model/CompositeExecutionObject.h\
  include/gingancl/model/CostFunctionDuration.h\
  include/gingancl/model/DurationFactory.h\
  include/gingancl/model/EndEventTransition.h\
  include/gingancl/model/EventTransition.h\
  include/gingancl/model/EventTransitionManager.h\
  include/gingancl/model/ExecutionObject.h\
  include/gingancl/model/ExecutionObjectSwitch.h\
  include/gingancl/model/FlexibleTimeMeasurement.h\
  include/gingancl/model/FocusSourceManager.h\
  include/gingancl/model/FormatterCausalLink.h\
  include/gingancl/model/FormatterDeviceRegion.h\
  include/gingancl/model/FormatterEvent.h\
  include/gingancl/model/FormatterLayout.h\
  include/gingancl/model/FormatterLink.h\
  include/gingancl/model/FormatterRegion.h\
  include/gingancl/model/LinearCostFunctionDuration.h\
  include/gingancl/model/LinkAction.h\
  include/gingancl/model/LinkActionProgressionListener.h\
  include/gingancl/model/LinkAndCompoundTriggerCondition.h\
  include/gingancl/model/LinkAssessment.h\
  include/gingancl/model/LinkAssessmentStatement.h\
  include/gingancl/model/LinkAssignmentAction.h\
  include/gingancl/model/LinkAttributeAssessment.h\
  include/gingancl/model/LinkCompoundAction.h\
  include/gingancl/model/LinkCompoundStatement.h\
  include/gingancl/model/LinkCompoundTriggerCondition.h\
  include/gingancl/model/LinkCondition.h\
  include/gingancl/model/LinkListener.h\
  include/gingancl/model/LinkRepeatAction.h\
  include/gingancl/model/LinkSimpleAction.h\
  include/gingancl/model/LinkStatement.h\
  include/gingancl/model/LinkTransitionTriggerCondition.h\
  include/gingancl/model/LinkTriggerCondition.h\
  include/gingancl/model/LinkTriggerListener.h\
  include/gingancl/model/LinkValueAssessment.h\
  include/gingancl/model/NodeNesting.h\
  include/gingancl/model/PresentationEvent.h\
  include/gingancl/model/ApplicationExecutionObject.h\
  include/gingancl/model/SelectionEvent.h\
  include/gingancl/model/SwitchEvent.h\
  include/gingancl/model/TimeMeasurement.h\
  include/gingancl/model/TransitionDispatcher.h\
  include/gingancl/model/UnflexibleDuration.h\
  include/gingancl/model/IAttributeValueMaintainer.h\
  include/gingancl/model/IFormatterEvent.h\
  include/gingancl/model/IEventListener.h\
  include/gingancl/model/ILinkActionListener.h\
  $(NULL)

if BUILD_MULTIDEVICE
multideviceincludedir = $(ginganclincludedir)/multidevice
multideviceinclude_HEADERS =\
  include/gingancl/multidevice/IFormatterMultiDevice.h\
  $(NULL)
endif

prefetchincludedir = $(ginganclincludedir)/prefetch
prefetchinclude_HEADERS =\
  include/gingancl/prefetch/IPrefetchManager.h\
  $(NULL)
endif

############################## Configuration ###############################
cfgformatterdir = $(gingaconfdir)/cfg/formatter
cfgformatter_DATA =\
  files/config/formatter/players/ctrldefs.ini\
  files/config/formatter/players/mimedefs.ini\
  $(NULL)

imgsformaterdir = $(gingaconfdir)/img/roller/
imgsformater_DATA =\
	files/img/loading.png\
	files/img/b1.png\
	files/img/b2.png\
	files/img/b3.png\
	files/img/b4.png\
	files/img/b6.png\
	files/img/b5.png\
	files/img/b7.png\
	files/img/b8.png\
	$(NULL)

mdimgsformaterdir = $(gingaconfdir)/md/
mdimgsformater_DATA =\
  files/img/active-device-reset.png\
  files/img/active-device.png\
	$(NULL)

