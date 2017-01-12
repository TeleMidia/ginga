include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  -DSTR_PREFIX='"$(prefix)"'\
  -DGINGA_LIBDIR='"$(gingalibdir)"'\
  -DGINGA_INI_PATH='"$(gingaconfdir)/ginga.ini"'\
  $(NULL)

AM_CXXFLAGS=\
 $(CURL_CFLAGS)\
 $(FFMPEG_CFLAGS)\
 $(JPEG_CFLAGS)\
 $(NCLUA_CFLAGS)\
 $(PTHREAD_CFLAGS)\
 $(SDL2IMAGE_CFLAGS)\
 $(SDL2TTF_CFLAGS)\
 $(SDL2_CFLAGS)\
 $(X11_CFLAGS)\
 $(XERCESC_CFLAGS)\
 $(NULL)

AM_LDFLAGS= -R $(gingalibdir) -rpath $(gingalibdir)\
 $(CURL_LIBS)\
 $(FFMPEG_LIBS)\
 $(JPEG_LIBS)\
 $(NCLUA_LIBS)\
 $(PTHREAD_LIBS)\
 $(SDL2IMAGE_LIBS)\
 $(SDL2TTF_LIBS)\
 $(SDL2_LIBS)\
 $(X11_LIBS)\
 $(XERCESC_LIBS)\
 $(NULL)

bin_PROGRAMS= ginga
ginga_SOURCES= $(src) main.cpp
src=
src+= ctxmgmt/ContextManager.cpp
src+= ctxmgmt/GingaUser.cpp
src+= ctxmgmt/SystemInfo.cpp
src+= gingancl/AnimationController.cpp
src+= gingancl/FormatterBaseDevice.cpp
src+= gingancl/FormatterConverter.cpp
src+= gingancl/FormatterFocusManager.cpp
src+= gingancl/FormatterLinkConverter.cpp
src+= gingancl/FormatterMediator.cpp
src+= gingancl/FormatterMultiDevice.cpp
src+= gingancl/FormatterScheduler.cpp
src+= gingancl/PrefetchManager.cpp
src+= gingancl/PrivateBaseContext.cpp
src+= gingancl/PrivateBaseManager.cpp
src+= gingancl/adaptation/ContextBase.cpp
src+= gingancl/adaptation/PresentationContext.cpp
src+= gingancl/adaptation/RuleAdapter.cpp
src+= gingancl/adapters/AVPlayerAdapter.cpp
src+= gingancl/adapters/ApplicationPlayerAdapter.cpp
src+= gingancl/adapters/ChannelPlayerAdapter.cpp
src+= gingancl/adapters/FormatterPlayerAdapter.cpp
src+= gingancl/adapters/ImagePlayerAdapter.cpp
src+= gingancl/adapters/LuaPlayerAdapter.cpp
src+= gingancl/adapters/MirrorPlayerAdapter.cpp
src+= gingancl/adapters/NCLPlayerAdapter.cpp
src+= gingancl/adapters/NominalEventMonitor.cpp
src+= gingancl/adapters/PlainTxtPlayerAdapter.cpp
src+= gingancl/adapters/PlayerAdapterManager.cpp
src+= gingancl/adapters/ProgramAVPlayerAdapter.cpp
src+= gingancl/adapters/SubtitlePlayerAdapter.cpp
src+= gingancl/adapters/TimePlayerAdapter.cpp
src+= gingancl/model/AnchorEvent.cpp
src+= gingancl/model/ApplicationExecutionObject.cpp
src+= gingancl/model/AttributionEvent.cpp
src+= gingancl/model/BeginEventTransition.cpp
src+= gingancl/model/CascadingDescriptor.cpp
src+= gingancl/model/CompositeExecutionObject.cpp
src+= gingancl/model/CostFunctionDuration.cpp
src+= gingancl/model/DurationFactory.cpp
src+= gingancl/model/EndEventTransition.cpp
src+= gingancl/model/EventTransition.cpp
src+= gingancl/model/EventTransitionManager.cpp
src+= gingancl/model/ExecutionObject.cpp
src+= gingancl/model/ExecutionObjectSwitch.cpp
src+= gingancl/model/FlexibleTimeMeasurement.cpp
src+= gingancl/model/FocusSourceManager.cpp
src+= gingancl/model/FormatterCausalLink.cpp
src+= gingancl/model/FormatterDeviceRegion.cpp
src+= gingancl/model/FormatterEvent.cpp
src+= gingancl/model/FormatterLayout.cpp
src+= gingancl/model/FormatterLink.cpp
src+= gingancl/model/FormatterRegion.cpp
src+= gingancl/model/LinearCostFunctionDuration.cpp
src+= gingancl/model/LinkAction.cpp
src+= gingancl/model/LinkAndCompoundTriggerCondition.cpp
src+= gingancl/model/LinkAssessmentStatement.cpp
src+= gingancl/model/LinkAssignmentAction.cpp
src+= gingancl/model/LinkAttributeAssessment.cpp
src+= gingancl/model/LinkCompoundAction.cpp
src+= gingancl/model/LinkCompoundStatement.cpp
src+= gingancl/model/LinkCompoundTriggerCondition.cpp
src+= gingancl/model/LinkRepeatAction.cpp
src+= gingancl/model/LinkSimpleAction.cpp
src+= gingancl/model/LinkTransitionTriggerCondition.cpp
src+= gingancl/model/LinkTriggerCondition.cpp
src+= gingancl/model/LinkValueAssessment.cpp
src+= gingancl/model/NodeNesting.cpp
src+= gingancl/model/PresentationEvent.cpp
src+= gingancl/model/SelectionEvent.cpp
src+= gingancl/model/SwitchEvent.cpp
src+= gingancl/model/TimeMeasurement.cpp
src+= gingancl/model/TransitionDispatcher.cpp
src+= gingancl/model/UnflexibleDuration.cpp
src+= ic/CurlInteractiveChannel.cpp
src+= ic/InteractiveChannelManager.cpp
src+= lssm/CommonCoreManager.cpp
src+= lssm/DataWrapperListener.cpp
src+= lssm/PresentationEngineManager.cpp
src+= lssm/StcWrapper.cpp
src+= mb/CodeMap.cpp
src+= mb/InputManager.cpp
src+= mb/LocalScreenManager.cpp
src+= mb/Matrix.cpp
src+= mb/SDL2ffmpeg.cpp
src+= mb/SDLAudioProvider.cpp
src+= mb/SDLConvert.cpp
src+= mb/SDLDeviceScreen.cpp
src+= mb/SDLEventBuffer.cpp
src+= mb/SDLFontProvider.cpp
src+= mb/SDLImageProvider.cpp
src+= mb/SDLInputEvent.cpp
src+= mb/SDLSurface.cpp
src+= mb/SDLVideoProvider.cpp
src+= mb/SDLWindow.cpp
src+= mb/ScreenManagerFactory.cpp
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
src+= nclconv/DOMTreeErrorReporter.cpp
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
src+= nclconv/XMLParsing.cpp
src+= player/AVPlayer.cpp
src+= player/ChannelPlayer.cpp
src+= player/ImagePlayer.cpp
src+= player/LuaPlayer.cpp
src+= player/NTPDatagram.cpp
src+= player/NTPPlayer.cpp
src+= player/PlainTxtPlayer.cpp
src+= player/Player.cpp
src+= player/ProgramAV.cpp
src+= player/ShowButton.cpp
src+= player/SrtPlayer.cpp
src+= player/TextPlayer.cpp
src+= player/UDPClient.cpp
src+= system/GingaLocatorFactory.cpp
src+= system/PracticalSocket.cpp
src+= system/Stc.cpp
src+= system/SystemCompat.cpp
src+= system/Thread.cpp
src+= util/Base64.cpp
src+= util/Color.cpp
src+= util/functions.cpp

if BUILD_BERKELIUM
AM_CXXFLAGS+= $(BERKELIUM_CFLAGS)
AM_LDFLAGS+= $(BERKELIUM_LIBS)
src+= gingancl/adapters/BerkeliumPlayerAdapter.cpp
src+= player/BerkeliumHandler.cpp
src+= player/BerkeliumPlayer.cpp
endif # BUILD_BERKELIUM

if BUILD_LIBBPG
AM_CXXFLAGS+= $(LIBBPG_CFLAGS)
AM_LDFLAGS+= $(LIBBPG_LIBS)
src+= SDLBpgDecoder.cpp
endif # BUILD_LIBBPG

if BUILD_ISDBT
AM_CPPFLAGS+=\
  -DGINGA_ISDBT_PATH='"$(gingaconfdir)/tuner/isdbt.ini"'\
  -DGINGA_TUNER_PATH='"$(gingaconfdir)/tuner/tuner.ini"'\
  $(NULL)
AM_CXXFLAGS+= $(EXPAT_CFLAGS)
AM_LDFLAGS+= $(EXPAT_LIBS)
src+= dataprocessing/DataProcessor.cpp
src+= dataprocessing/EPGProcessor.cpp
src+= dataprocessing/FilterManager.cpp
src+= dataprocessing/SectionFilter.cpp
src+= dataprocessing/dsmcc/Binding.cpp
src+= dataprocessing/dsmcc/Biop.cpp
src+= dataprocessing/dsmcc/Crc.cpp
src+= dataprocessing/dsmcc/DSMCCSectionPayload.cpp
src+= dataprocessing/dsmcc/DownloadDataBlock.cpp
src+= dataprocessing/dsmcc/DownloadInfoIndication.cpp
src+= dataprocessing/dsmcc/DownloadServerInitiate.cpp
src+= dataprocessing/dsmcc/DsmccMessageHeader.cpp
src+= dataprocessing/dsmcc/Ior.cpp
src+= dataprocessing/dsmcc/MessageProcessor.cpp
src+= dataprocessing/dsmcc/Module.cpp
src+= dataprocessing/dsmcc/MpegDescriptor.cpp
src+= dataprocessing/dsmcc/NPTEndpoint.cpp
src+= dataprocessing/dsmcc/NPTProcessor.cpp
src+= dataprocessing/dsmcc/NPTReference.cpp
src+= dataprocessing/dsmcc/Object.cpp
src+= dataprocessing/dsmcc/ObjectProcessor.cpp
src+= dataprocessing/dsmcc/ServiceDomain.cpp
src+= dataprocessing/dsmcc/StreamEvent.cpp
src+= dataprocessing/dsmcc/StreamMode.cpp
src+= dataprocessing/dsmcc/TimeBaseClock.cpp
src+= dataprocessing/ncl/DataFile.cpp
src+= dataprocessing/ncl/EventDescriptor.cpp
src+= dataprocessing/ncl/Metadata.cpp
src+= dataprocessing/ncl/NCLMetadataParser.cpp
src+= dataprocessing/ncl/NCLSectionProcessor.cpp
src+= tsparser/AIT.cpp
src+= tsparser/Application.cpp
src+= tsparser/ApplicationDescriptor.cpp
src+= tsparser/ApplicationLocationDescriptor.cpp
src+= tsparser/ApplicationNameDescriptor.cpp
src+= tsparser/AudioComponentDescriptor.cpp
src+= tsparser/ComponentDescriptor.cpp
src+= tsparser/ContentAvailabilityDescriptor.cpp
src+= tsparser/ContentDescriptor.cpp
src+= tsparser/DataContentDescriptor.cpp
src+= tsparser/Demuxer.cpp
src+= tsparser/DigitalCCDescriptor.cpp
src+= tsparser/EventInfo.cpp
src+= tsparser/ExtendedEventDescriptor.cpp
src+= tsparser/FrontendFilter.cpp
src+= tsparser/LocalTimeOffsetDescriptor.cpp
src+= tsparser/LogoTransmissionDescriptor.cpp
src+= tsparser/PSIFilter.cpp
src+= tsparser/ParentalRatingDescriptor.cpp
src+= tsparser/Pat.cpp
src+= tsparser/PesFilter.cpp
src+= tsparser/PipeFilter.cpp
src+= tsparser/Pmt.cpp
src+= tsparser/PrefetchDescriptor.cpp
src+= tsparser/SeriesDescriptor.cpp
src+= tsparser/ServiceDescriptor.cpp
src+= tsparser/ServiceInfo.cpp
src+= tsparser/ShortEventDescriptor.cpp
src+= tsparser/StreamIdentifierDescriptor.cpp
src+= tsparser/TOT.cpp
src+= tsparser/TSAdaptationField.cpp
src+= tsparser/TSPacket.cpp
src+= tsparser/TransportProtocolDescriptor.cpp
src+= tsparser/TransportSection.cpp
src+= tuner/Channel.cpp
src+= tuner/FileSystemProvider.cpp
src+= tuner/MulticastProvider.cpp
src+= tuner/NetworkInterface.cpp
src+= tuner/NetworkProvider.cpp
src+= tuner/Tuner.cpp
src+= tuner/UnicastProvider.cpp
if BUILD_LINUXDVB
src+= tuner/ISDBTFrontend.cpp
src+= tuner/ISDBTProvider.cpp
src+= tuner/RingBuffer.c
endif # BUILD_LINUXDVB
endif # BUILD_ISDBT

if BUILD_MULTIDEVICE
AM_CXXFLAGS+= $(LIBZIP_CFLAGS)
AM_LDFLAGS+= $(LIBZIP_LIBS)
src+= gingancl/FormatterActiveDevice.cpp
src+= gingancl/FormatterPassiveDevice.cpp
src+= multidevice/ActiveDeviceDomain.cpp
src+= multidevice/ActiveDeviceService.cpp
src+= multidevice/BaseDeviceDomain.cpp
src+= multidevice/BaseDeviceService.cpp
src+= multidevice/BroadcastDualSocketService.cpp
src+= multidevice/BroadcastSocketService.cpp
src+= multidevice/DeviceDomain.cpp
src+= multidevice/DeviceService.cpp
src+= multidevice/MulticastSocketService.cpp
src+= multidevice/NetworkUtil.cpp
src+= multidevice/PassiveDeviceDomain.cpp
src+= multidevice/PassiveDeviceService.cpp
src+= multidevice/RemoteDevice.cpp
src+= multidevice/RemoteDeviceManager.cpp
src+= multidevice/RemoteEventService.cpp
src+= multidevice/TcpClientConnection.cpp
src+= multidevice/TcpSocketService.cpp
endif # BUILD_MULTIDEVICE

if BUILD_SSML
AM_CXXFLAGS+= $(ESPEAK_CFLAGS)
AM_LDFLAGS+= $(ESPEAK_LIBS)
src+= gingancl/adapters/SsmlPlayerAdapter.cpp
src+= player/SsmlPlayer.cpp
endif # BUILD_SSML

if BUILD_SVG
AM_CXXFLAGS+= $(LIBRSVG_CFLAGS)
AM_LDFLAGS+= $(LIBRSVG_LIBS)
src+= SDLSvgDecoder.cpp
endif # BUILD_SVG
