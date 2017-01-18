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
src+= gingancl/AdapterAVPlayer.cpp
src+= gingancl/AdapterApplicationPlayer.cpp
src+= gingancl/AdapterChannelPlayer.cpp
src+= gingancl/AdapterFormatterPlayer.cpp
src+= gingancl/AdapterImagePlayer.cpp
src+= gingancl/AdapterLuaPlayer.cpp
src+= gingancl/AdapterMirrorPlayer.cpp
src+= gingancl/AdapterNCLPlayer.cpp
src+= gingancl/AdapterPlainTxtPlayer.cpp
src+= gingancl/AdapterPlayerManager.cpp
src+= gingancl/AdapterProgramAVPlayer.cpp
src+= gingancl/AdapterSubtitlePlayer.cpp
#src+= gingancl/AdapterTimePlayer.cpp
src+= gingancl/AnimationController.cpp
src+= gingancl/FormatterBaseDevice.cpp
src+= gingancl/FormatterConverter.cpp
src+= gingancl/FormatterFocusManager.cpp
src+= gingancl/FormatterLinkConverter.cpp
src+= gingancl/FormatterMediator.cpp
src+= gingancl/FormatterMultiDevice.cpp
src+= gingancl/FormatterScheduler.cpp
src+= gingancl/NclAnchorEvent.cpp
src+= gingancl/NclApplicationExecutionObject.cpp
src+= gingancl/NclAttributionEvent.cpp
src+= gingancl/NclBeginEventTransition.cpp
src+= gingancl/NclCascadingDescriptor.cpp
src+= gingancl/NclCompositeExecutionObject.cpp
src+= gingancl/NclEndEventTransition.cpp
src+= gingancl/NclEventTransition.cpp
src+= gingancl/NclEventTransitionManager.cpp
src+= gingancl/NclExecutionObject.cpp
src+= gingancl/NclExecutionObjectSwitch.cpp
src+= gingancl/NclFocusSourceManager.cpp
src+= gingancl/NclFormatterCausalLink.cpp
src+= gingancl/NclFormatterDeviceRegion.cpp
src+= gingancl/NclFormatterEvent.cpp
src+= gingancl/NclFormatterLayout.cpp
src+= gingancl/NclFormatterLink.cpp
src+= gingancl/NclFormatterRegion.cpp
src+= gingancl/NclLinkAction.cpp
src+= gingancl/NclLinkAndCompoundTriggerCondition.cpp
src+= gingancl/NclLinkAssessmentStatement.cpp
src+= gingancl/NclLinkAssignmentAction.cpp
src+= gingancl/NclLinkAttributeAssessment.cpp
src+= gingancl/NclLinkCompoundAction.cpp
src+= gingancl/NclLinkCompoundStatement.cpp
src+= gingancl/NclLinkCompoundTriggerCondition.cpp
src+= gingancl/NclLinkRepeatAction.cpp
src+= gingancl/NclLinkSimpleAction.cpp
src+= gingancl/NclLinkTransitionTriggerCondition.cpp
src+= gingancl/NclLinkTriggerCondition.cpp
src+= gingancl/NclLinkValueAssessment.cpp
src+= gingancl/NclNodeNesting.cpp
src+= gingancl/NclPresentationEvent.cpp
src+= gingancl/NclSelectionEvent.cpp
src+= gingancl/NclSwitchEvent.cpp
src+= gingancl/NominalEventMonitor.cpp
src+= gingancl/PrefetchManager.cpp
src+= gingancl/PresentationContext.cpp
src+= gingancl/PrivateBaseContext.cpp
src+= gingancl/PrivateBaseManager.cpp
src+= gingancl/RuleAdapter.cpp
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
src+= gingancl/AdapterBerkeliumPlayer.cpp
src+= player/BerkeliumHandler.cpp
src+= player/BerkeliumPlayer.cpp
endif # WITH_BERKELIUM

if WITH_ESPEAK
AM_CXXFLAGS+= $(ESPEAK_CFLAGS)
AM_LDFLAGS+= $(ESPEAK_LIBS)
src+= gingancl/AdapterSsmlPlayer.cpp
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
