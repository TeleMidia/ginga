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
 $(GLIB_CFLAGS)\
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
 $(GLIB_LIBS)\
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
#src+= gingancl/adapters/TimePlayerAdapter.cpp
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
src+= lssm/PresentationEngineManager.cpp
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
src+= ncl/ContentTypeManager.cpp
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
#src+= player/NTPPlayer.cpp
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
src+= lssm/DataWrapperListener.cpp
src+= lssm/StcWrapper.cpp
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
