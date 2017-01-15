include $(top_srcdir)/build-aux/Makefile.am.common

AM_CPPFLAGS=\
  -DGINGA_DATADIR='"$(pkgdatadir)/"'\
  -DGINGA_BUTTON_DATADIR='"$(buttondir)/"'\
  -DGINGA_CONTEXTMANAGER_DATADIR='"$(contextmanagerdir)/"'\
  -DGINGA_FONT_DATADIR='"$(fontdir)"'\
  -DGINGA_FORMATTER_DATADIR='"$(formatterdir)/"'\
  -DGINGA_FORMATTERIMG_DATADIR='"$(formatterimgdir)/"'\
  -DGINGA_FORMATTERMD_DATADIR='"$(formattermddir)/"'\
  -DGINGA_MB_DATADIR='"$(mbdir)/"'\
  -DGINGA_TUNER_DATADIR='"$(tunerdir)/"'\
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
 $(XERCESC_CFLAGS)\
 $(NULL)

AM_LDFLAGS= -R $(libdir) -rpath $(libdir)\
 $(CURL_LIBS)\
 $(FFMPEG_LIBS)\
 $(JPEG_LIBS)\
 $(NCLUA_LIBS)\
 $(PTHREAD_LIBS)\
 $(SDL2IMAGE_LIBS)\
 $(SDL2TTF_LIBS)\
 $(SDL2_LIBS)\
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
src+= gingancl/model/EndEventTransition.cpp
src+= gingancl/model/EventTransition.cpp
src+= gingancl/model/EventTransitionManager.cpp
src+= gingancl/model/ExecutionObject.cpp
src+= gingancl/model/ExecutionObjectSwitch.cpp
src+= gingancl/model/FocusSourceManager.cpp
src+= gingancl/model/FormatterCausalLink.cpp
src+= gingancl/model/FormatterDeviceRegion.cpp
src+= gingancl/model/FormatterEvent.cpp
src+= gingancl/model/FormatterLayout.cpp
src+= gingancl/model/FormatterLink.cpp
src+= gingancl/model/FormatterRegion.cpp
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
src+= ic/CurlInteractiveChannel.cpp
src+= ic/InteractiveChannelManager.cpp
src+= lssm/CommonCoreManager.cpp
src+= lssm/DataWrapperListener.cpp
src+= lssm/PresentationEngineManager.cpp
src+= lssm/StcWrapper.cpp
src+= mb/CodeMap.cpp
src+= mb/InputManager.cpp
src+= mb/LocalScreenManager.cpp
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
src+= ncl/connectors/Action.cpp
src+= ncl/connectors/AssessmentStatement.cpp
src+= ncl/connectors/AttributeAssessment.cpp
src+= ncl/connectors/CausalConnector.cpp
src+= ncl/connectors/CompoundAction.cpp
src+= ncl/connectors/CompoundCondition.cpp
src+= ncl/connectors/CompoundStatement.cpp
src+= ncl/connectors/Connector.cpp
src+= ncl/connectors/ConnectorBase.cpp
src+= ncl/connectors/EventUtil.cpp
src+= ncl/connectors/Role.cpp
src+= ncl/connectors/SimpleAction.cpp
src+= ncl/connectors/SimpleCondition.cpp
src+= ncl/connectors/TriggerExpression.cpp
src+= ncl/connectors/ValueAssessment.cpp
src+= ncl/descriptor/Descriptor.cpp
src+= ncl/descriptor/DescriptorBase.cpp
src+= ncl/interfaces/Anchor.cpp
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

if WITH_BERKELIUM
AM_CXXFLAGS+= $(BERKELIUM_CFLAGS)
AM_LDFLAGS+= $(BERKELIUM_LIBS)
src+= gingancl/adapters/BerkeliumPlayerAdapter.cpp
src+= player/BerkeliumHandler.cpp
src+= player/BerkeliumPlayer.cpp
endif # WITH_BERKELIUM

if WITH_ESPEAK
AM_CXXFLAGS+= $(ESPEAK_CFLAGS)
AM_LDFLAGS+= $(ESPEAK_LIBS)
src+= gingancl/adapters/SsmlPlayerAdapter.cpp
src+= player/SsmlPlayer.cpp
endif # WITH_ESPEAK

if WITH_ISDBT
AM_CPPFLAGS+=\
  -DGINGA_ISDBT_PATH='"$(gingaconfdir)/tuner/isdbt.ini"'\
  -DGINGA_TUNER_PATH='"$(gingaconfdir)/tuner/tuner.ini"'\
  $(NULL)
AM_CXXFLAGS+= $(ISDBT_CFLAGS)
AM_LDFLAGS+= $(ISDBT_LIBS)
src+= isdbt-dataproc/DataProcessor.cpp
src+= isdbt-dataproc/EPGProcessor.cpp
src+= isdbt-dataproc/FilterManager.cpp
src+= isdbt-dataproc/SectionFilter.cpp
src+= isdbt-dataproc/dsmcc/Binding.cpp
src+= isdbt-dataproc/dsmcc/Biop.cpp
src+= isdbt-dataproc/dsmcc/Crc.cpp
src+= isdbt-dataproc/dsmcc/DSMCCSectionPayload.cpp
src+= isdbt-dataproc/dsmcc/DownloadDataBlock.cpp
src+= isdbt-dataproc/dsmcc/DownloadInfoIndication.cpp
src+= isdbt-dataproc/dsmcc/DownloadServerInitiate.cpp
src+= isdbt-dataproc/dsmcc/DsmccMessageHeader.cpp
src+= isdbt-dataproc/dsmcc/Ior.cpp
src+= isdbt-dataproc/dsmcc/MessageProcessor.cpp
src+= isdbt-dataproc/dsmcc/Module.cpp
src+= isdbt-dataproc/dsmcc/MpegDescriptor.cpp
src+= isdbt-dataproc/dsmcc/NPTEndpoint.cpp
src+= isdbt-dataproc/dsmcc/NPTProcessor.cpp
src+= isdbt-dataproc/dsmcc/NPTReference.cpp
src+= isdbt-dataproc/dsmcc/Object.cpp
src+= isdbt-dataproc/dsmcc/ObjectProcessor.cpp
src+= isdbt-dataproc/dsmcc/ServiceDomain.cpp
src+= isdbt-dataproc/dsmcc/StreamEvent.cpp
src+= isdbt-dataproc/dsmcc/StreamMode.cpp
src+= isdbt-dataproc/dsmcc/TimeBaseClock.cpp
src+= isdbt-dataproc/ncl/DataFile.cpp
src+= isdbt-dataproc/ncl/EventDescriptor.cpp
src+= isdbt-dataproc/ncl/Metadata.cpp
src+= isdbt-dataproc/ncl/NCLMetadataParser.cpp
src+= isdbt-dataproc/ncl/NCLSectionProcessor.cpp
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
src+= isdbt-tuner/RingBuffer.c
endif # WITH_LINUXDVB
endif # WITH_ISDBT

if WITH_LIBBPG
AM_CXXFLAGS+= $(LIBBPG_CFLAGS)
AM_LDFLAGS+= $(LIBBPG_LIBS)
src+= SDLBpgDecoder.cpp
endif # WITH_LIBBPG

if WITH_MULTIDEVICE
AM_CXXFLAGS+= $(MULTIDEVICE_CFLAGS)
AM_LDFLAGS+= $(MULTIDEVICE_LIBS)
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
endif # WITH_MULTIDEVICE

if WITH_LIBRSVG
AM_CXXFLAGS+= $(LIBRSVG_CFLAGS)
AM_LDFLAGS+= $(LIBRSVG_LIBS)
src+= mb/SDLSvgDecoder.cpp
endif # WITH_LIBRSVG
