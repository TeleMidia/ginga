cmake_minimum_required(VERSION 3.2)
project(ginga)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENTDIR ON)

find_package(XercesC 2.7 REQUIRED)
find_package(FFmpeg REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(CURL REQUIRED)
find_package(GLIB 2.32 REQUIRED)

find_package(Cairo REQUIRED)
find_package(GDKPixBuf REQUIRED)
find_package(RSVG REQUIRED)
find_package(LibZip REQUIRED)

find_package(LinuxDVB)

#add_definitions(-DPACKAGE_VERSION="1.0")
#add_definitions(-DWITH_ISDBT=0)
#add_definitions(-DWITH_LINUXDVB)
#add_definitions(-DWITH_MULTIDEVICE)

add_definitions(-DGINGA_DATADIR="${CMAKE_BINARY_DIR}/data/ini/")
add_definitions(-DGINGA_BUTTON_DATADIR="${CMAKE_BINARY_DIR}/data/img/button/")
add_definitions(-DGINGA_CONTEXTMANAGER_DATADIR="${CMAKE_BINARY_DIR}/data/contextmanager/")
add_definitions(-DGINGA_FONT_DATADIR="${CMAKE_BINARY_DIR}/data/font/")
add_definitions(-DGINGA_FORMATTER_DATADIR="${CMAKE_BINARY_DIR}/data/")
add_definitions(-DGINGA_FORMATTERIMG_DATADIR="${CMAKE_BINARY_DIR}/data/img/roller/")
add_definitions(-DGINGA_FORMATTERMD_DATADIR="${CMAKE_BINARY_DIR}/data/md/")
add_definitions(-DGINGA_MB_DATADIR="${CMAKE_BINARY_DIR}/data/mb/")
add_definitions(-DGINGA_ISDBT_PATH="${CMAKE_BINARY_DIR}/data/tuner/isdbt.ini")
add_definitions(-DGINGA_TUNER_DATADIR="${CMAKE_BINARY_DIR}/data/tuner")
add_definitions(-DGINGA_TUNER_PATH="${CMAKE_BINARY_DIR}/data/tuner/tuner.ini")

# NCLUA download/configure/build/install
include(ExternalProject)
ExternalProject_Add(nclua_build
  GIT_REPOSITORY "https://github.com/gflima/nclua.git"
  GIT_TAG master
  UPDATE_COMMAND ""

  CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap && <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

ExternalProject_Get_Property(nclua_build install_dir)
set(NCLUA_INCLUDE_DIRS "${install_dir}/include")
link_directories("${install_dir}/lib")
#end NCLUA

add_executable(ginga
  ../src/ginga.h

  ../src/ctxmgmt/ContextManager.cpp
  ../src/ctxmgmt/GingaUser.cpp
  ../src/ctxmgmt/SystemInfo.cpp

  ../src/formatter/AdapterAVPlayer.cpp
  ../src/formatter/AdapterApplicationPlayer.cpp
  ../src/formatter/AdapterChannelPlayer.cpp
  ../src/formatter/AdapterFormatterPlayer.cpp
  ../src/formatter/AdapterImagePlayer.cpp
  ../src/formatter/AdapterLuaPlayer.cpp
  ../src/formatter/AdapterMirrorPlayer.cpp
  ../src/formatter/AdapterNCLPlayer.cpp
  ../src/formatter/AdapterNewVideoPlayer.cpp
  ../src/formatter/AdapterPlainTxtPlayer.cpp
  ../src/formatter/AdapterPlayerManager.cpp
  ../src/formatter/AdapterProgramAVPlayer.cpp
  ../src/formatter/AdapterSubtitlePlayer.cpp
  ../src/formatter/AnimationController.cpp
  ../src/formatter/FormatterBaseDevice.cpp
  ../src/formatter/FormatterConverter.cpp
  ../src/formatter/FormatterFocusManager.cpp
  ../src/formatter/FormatterLinkConverter.cpp
  ../src/formatter/FormatterMediator.cpp
  ../src/formatter/FormatterMultiDevice.cpp
  ../src/formatter/FormatterScheduler.cpp
  ../src/formatter/NclAnchorEvent.cpp
  ../src/formatter/NclApplicationExecutionObject.cpp
  ../src/formatter/NclAttributionEvent.cpp
  ../src/formatter/NclBeginEventTransition.cpp
  ../src/formatter/NclCascadingDescriptor.cpp
  ../src/formatter/NclCompositeExecutionObject.cpp
  ../src/formatter/NclEndEventTransition.cpp
  ../src/formatter/NclEventTransition.cpp
  ../src/formatter/NclEventTransitionManager.cpp
  ../src/formatter/NclExecutionObject.cpp
  ../src/formatter/NclExecutionObjectSwitch.cpp
  ../src/formatter/NclFocusSourceManager.cpp
  ../src/formatter/NclFormatterCausalLink.cpp
  ../src/formatter/NclFormatterDeviceRegion.cpp
  ../src/formatter/NclFormatterEvent.cpp
  ../src/formatter/NclFormatterLayout.cpp
  ../src/formatter/NclFormatterLink.cpp
  ../src/formatter/NclFormatterRegion.cpp
  ../src/formatter/NclLinkAction.cpp
  ../src/formatter/NclLinkAndCompoundTriggerCondition.cpp
  ../src/formatter/NclLinkAssessmentStatement.cpp
  ../src/formatter/NclLinkAssignmentAction.cpp
  ../src/formatter/NclLinkAttributeAssessment.cpp
  ../src/formatter/NclLinkCompoundAction.cpp
  ../src/formatter/NclLinkCompoundStatement.cpp
  ../src/formatter/NclLinkCompoundTriggerCondition.cpp
  ../src/formatter/NclLinkRepeatAction.cpp
  ../src/formatter/NclLinkSimpleAction.cpp
  ../src/formatter/NclLinkTransitionTriggerCondition.cpp
  ../src/formatter/NclLinkTriggerCondition.cpp
  ../src/formatter/NclLinkValueAssessment.cpp
  ../src/formatter/NclNodeNesting.cpp
  ../src/formatter/NclPresentationEvent.cpp
  ../src/formatter/NclSelectionEvent.cpp
  ../src/formatter/NclSwitchEvent.cpp
  ../src/formatter/NominalEventMonitor.cpp
  ../src/formatter/PrefetchManager.cpp
  ../src/formatter/PresentationContext.cpp
  ../src/formatter/PrivateBaseContext.cpp
  ../src/formatter/PrivateBaseManager.cpp
  ../src/formatter/RuleAdapter.cpp

  ../src/ic/CurlInteractiveChannel.cpp
  ../src/ic/InteractiveChannelManager.cpp

  ../src/lssm/CommonCoreManager.cpp
  ../src/lssm/PresentationEngineManager.cpp

  ../src/mb/AudioProvider.cpp
  ../src/mb/Key.cpp
  ../src/mb/Display.cpp
  ../src/mb/FontProvider.cpp
  ../src/mb/InputManager.cpp
  ../src/mb/SDL2ffmpeg.cpp
  ../src/mb/SDLConvert.cpp
  ../src/mb/SDLEventBuffer.cpp
  ../src/mb/SDLInputEvent.cpp
  ../src/mb/SDLSurface.cpp
  ../src/mb/SDLWindow.cpp
  ../src/mb/VideoProvider.cpp

  ../src/ncl/AbsoluteReferenceContent.cpp
  ../src/ncl/Action.cpp
  ../src/ncl/Anchor.cpp
  ../src/ncl/Animation.cpp
  ../src/ncl/AssessmentStatement.cpp
  ../src/ncl/AttributeAssessment.cpp
  ../src/ncl/Base.cpp
  ../src/ncl/Bind.cpp
  ../src/ncl/CausalConnector.cpp
  ../src/ncl/CausalLink.cpp
  ../src/ncl/Comparator.cpp
  ../src/ncl/CompositeNode.cpp
  ../src/ncl/CompositeRule.cpp
  ../src/ncl/CompoundAction.cpp
  ../src/ncl/CompoundCondition.cpp
  ../src/ncl/CompoundStatement.cpp
  ../src/ncl/Connector.cpp
  ../src/ncl/ConnectorBase.cpp
  ../src/ncl/Content.cpp
  ../src/ncl/ContentAnchor.cpp
  ../src/ncl/ContentNode.cpp
  ../src/ncl/ContentTypeManager.cpp
  ../src/ncl/ContextNode.cpp
  ../src/ncl/Descriptor.cpp
  ../src/ncl/DescriptorBase.cpp
  ../src/ncl/DescriptorSwitch.cpp
  ../src/ncl/DeviceLayout.cpp
  ../src/ncl/Entity.cpp
  ../src/ncl/EventUtil.cpp
  ../src/ncl/FocusDecoration.cpp
  ../src/ncl/IntervalAnchor.cpp
  ../src/ncl/KeyNavigation.cpp
  ../src/ncl/LabeledAnchor.cpp
  ../src/ncl/LambdaAnchor.cpp
  ../src/ncl/LayoutRegion.cpp
  ../src/ncl/Link.cpp
  ../src/ncl/Meta.cpp
  ../src/ncl/Metadata.cpp
  ../src/ncl/NclDocument.cpp
  ../src/ncl/Node.cpp
  ../src/ncl/NodeEntity.cpp
  ../src/ncl/Parameter.cpp
  ../src/ncl/Port.cpp
  ../src/ncl/PrivateBase.cpp
  ../src/ncl/PropertyAnchor.cpp
  ../src/ncl/RectangleSpatialAnchor.cpp
  ../src/ncl/ReferNode.cpp
  ../src/ncl/ReferenceContent.cpp
  ../src/ncl/ReferredNode.cpp
  ../src/ncl/RegionBase.cpp
  ../src/ncl/RelativeTimeIntervalAnchor.cpp
  ../src/ncl/Role.cpp
  ../src/ncl/Rule.cpp
  ../src/ncl/RuleBase.cpp
  ../src/ncl/SampleIntervalAnchor.cpp
  ../src/ncl/SimpleAction.cpp
  ../src/ncl/SimpleCondition.cpp
  ../src/ncl/SimpleRule.cpp
  ../src/ncl/SwitchContent.cpp
  ../src/ncl/SwitchNode.cpp
  ../src/ncl/SwitchPort.cpp
  ../src/ncl/TextAnchor.cpp
  ../src/ncl/Transition.cpp
  ../src/ncl/TransitionBase.cpp
  ../src/ncl/TransitionUtil.cpp
  ../src/ncl/TriggerExpression.cpp
  ../src/ncl/ValueAssessment.cpp

  ../src/nclconv/DOMTreeErrorReporter.cpp
  ../src/nclconv/DocumentParser.cpp
  ../src/nclconv/NclComponentsConverter.cpp
  ../src/nclconv/NclComponentsParser.cpp
  ../src/nclconv/NclConnectorsConverter.cpp
  ../src/nclconv/NclConnectorsParser.cpp
  ../src/nclconv/NclDocumentConverter.cpp
  ../src/nclconv/NclDocumentParser.cpp
  ../src/nclconv/NclImportConverter.cpp
  ../src/nclconv/NclImportParser.cpp
  ../src/nclconv/NclInterfacesConverter.cpp
  ../src/nclconv/NclInterfacesParser.cpp
  ../src/nclconv/NclLayoutConverter.cpp
  ../src/nclconv/NclLayoutParser.cpp
  ../src/nclconv/NclLinkingConverter.cpp
  ../src/nclconv/NclLinkingParser.cpp
  ../src/nclconv/NclMetainformationConverter.cpp
  ../src/nclconv/NclMetainformationParser.cpp
  ../src/nclconv/NclPresentationControlConverter.cpp
  ../src/nclconv/NclPresentationControlParser.cpp
  ../src/nclconv/NclPresentationSpecConverter.cpp
  ../src/nclconv/NclPresentationSpecificationParser.cpp
  ../src/nclconv/NclStructureConverter.cpp
  ../src/nclconv/NclStructureParser.cpp
  ../src/nclconv/NclTransitionConverter.cpp
  ../src/nclconv/NclTransitionParser.cpp
  ../src/nclconv/XMLParsing.cpp

  ../src/player/AVPlayer.cpp
  ../src/player/ChannelPlayer.cpp
  ../src/player/ImagePlayer.cpp
  ../src/player/LuaPlayer.cpp
  ../src/player/NewVideoPlayer.cpp
  ../src/player/PlainTxtPlayer.cpp
  ../src/player/Player.cpp
  ../src/player/ProgramAV.cpp
  ../src/player/ShowButton.cpp
  ../src/player/SrtPlayer.cpp
  ../src/player/TextPlayer.cpp

  ../src/system/GingaLocatorFactory.cpp
  ../src/system/PracticalSocket.cpp
  ../src/system/Stc.cpp
  ../src/system/SystemCompat.cpp
  ../src/system/Thread.cpp

  ../src/util/Base64.cpp
  ../src/util/Color.cpp
  ../src/util/functions.cpp

#  WITH_ISDBT
  ../src/lssm/DataWrapperListener.cpp
  ../src/lssm/StcWrapper.cpp
  ../src/isdbt-dataproc/DataProcessor.cpp
  ../src/isdbt-dataproc/DsmccBinding.cpp
  ../src/isdbt-dataproc/DsmccBiop.cpp
  ../src/isdbt-dataproc/DsmccCrc.cpp
  ../src/isdbt-dataproc/DsmccDownloadDataBlock.cpp
  ../src/isdbt-dataproc/DsmccDownloadInfoIndication.cpp
  ../src/isdbt-dataproc/DsmccDownloadServerInitiate.cpp
  ../src/isdbt-dataproc/DsmccIor.cpp
  ../src/isdbt-dataproc/DsmccMessageHeader.cpp
  ../src/isdbt-dataproc/DsmccMessageProcessor.cpp
  ../src/isdbt-dataproc/DsmccModule.cpp
  ../src/isdbt-dataproc/DsmccMpegDescriptor.cpp
  ../src/isdbt-dataproc/DsmccNPTEndpoint.cpp
  ../src/isdbt-dataproc/DsmccNPTProcessor.cpp
  ../src/isdbt-dataproc/DsmccNPTReference.cpp
  ../src/isdbt-dataproc/DsmccObject.cpp
  ../src/isdbt-dataproc/DsmccObjectProcessor.cpp
  ../src/isdbt-dataproc/DsmccSectionPayload.cpp
  ../src/isdbt-dataproc/DsmccServiceDomain.cpp
  ../src/isdbt-dataproc/DsmccStreamEvent.cpp
  ../src/isdbt-dataproc/DsmccStreamMode.cpp
  ../src/isdbt-dataproc/DsmccTimeBaseClock.cpp
  ../src/isdbt-dataproc/EPGProcessor.cpp
  ../src/isdbt-dataproc/FilterManager.cpp
  ../src/isdbt-dataproc/NCLDataFile.cpp
  ../src/isdbt-dataproc/NCLEventDescriptor.cpp
  ../src/isdbt-dataproc/NCLMetadata.cpp
  ../src/isdbt-dataproc/NCLMetadataParser.cpp
  ../src/isdbt-dataproc/NCLSectionProcessor.cpp
  ../src/isdbt-dataproc/SectionFilter.cpp
  ../src/isdbt-tsparser/AIT.cpp
  ../src/isdbt-tsparser/Application.cpp
  ../src/isdbt-tsparser/ApplicationDescriptor.cpp
  ../src/isdbt-tsparser/ApplicationLocationDescriptor.cpp
  ../src/isdbt-tsparser/ApplicationNameDescriptor.cpp
  ../src/isdbt-tsparser/AudioComponentDescriptor.cpp
  ../src/isdbt-tsparser/ComponentDescriptor.cpp
  ../src/isdbt-tsparser/ContentAvailabilityDescriptor.cpp
  ../src/isdbt-tsparser/ContentDescriptor.cpp
  ../src/isdbt-tsparser/DataContentDescriptor.cpp
  ../src/isdbt-tsparser/Demuxer.cpp
  ../src/isdbt-tsparser/DigitalCCDescriptor.cpp
  ../src/isdbt-tsparser/EventInfo.cpp
  ../src/isdbt-tsparser/ExtendedEventDescriptor.cpp
  ../src/isdbt-tsparser/FrontendFilter.cpp
  ../src/isdbt-tsparser/LocalTimeOffsetDescriptor.cpp
  ../src/isdbt-tsparser/LogoTransmissionDescriptor.cpp
  ../src/isdbt-tsparser/PSIFilter.cpp
  ../src/isdbt-tsparser/ParentalRatingDescriptor.cpp
  ../src/isdbt-tsparser/Pat.cpp
  ../src/isdbt-tsparser/PesFilter.cpp
  ../src/isdbt-tsparser/PipeFilter.cpp
  ../src/isdbt-tsparser/Pmt.cpp
  ../src/isdbt-tsparser/PrefetchDescriptor.cpp
  ../src/isdbt-tsparser/SeriesDescriptor.cpp
  ../src/isdbt-tsparser/ServiceDescriptor.cpp
  ../src/isdbt-tsparser/ServiceInfo.cpp
  ../src/isdbt-tsparser/ShortEventDescriptor.cpp
  ../src/isdbt-tsparser/StreamIdentifierDescriptor.cpp
  ../src/isdbt-tsparser/TOT.cpp
  ../src/isdbt-tsparser/TSAdaptationField.cpp
  ../src/isdbt-tsparser/TSPacket.cpp
  ../src/isdbt-tsparser/TransportProtocolDescriptor.cpp
  ../src/isdbt-tsparser/TransportSection.cpp

  ../src/isdbt-tuner/Channel.cpp
  ../src/isdbt-tuner/FileSystemProvider.cpp
  ../src/isdbt-tuner/MulticastProvider.cpp
  ../src/isdbt-tuner/NetworkInterface.cpp
  ../src/isdbt-tuner/NetworkProvider.cpp
  ../src/isdbt-tuner/Tuner.cpp

#  WITH_MULTIDEVICE
  ../src/formatter/FormatterActiveDevice.cpp
  ../src/multidev/ActiveDeviceDomain.cpp
  ../src/multidev/ActiveDeviceService.cpp
  ../src/multidev/BaseDeviceDomain.cpp
  ../src/multidev/BaseDeviceService.cpp
  ../src/multidev/BroadcastDualSocketService.cpp
  ../src/multidev/BroadcastSocketService.cpp
  ../src/multidev/DeviceDomain.cpp
  ../src/multidev/DeviceService.cpp
  ../src/multidev/MulticastSocketService.cpp
  ../src/multidev/NetworkUtil.cpp
  ../src/multidev/RemoteDevice.cpp
  ../src/multidev/RemoteDeviceManager.cpp
  ../src/multidev/RemoteEventService.cpp
  ../src/multidev/TcpClientConnection.cpp
  ../src/multidev/TcpSocketService.cpp

# WITH_LINUXDVB
  ../src/isdbt-tuner/ISDBTFrontend.cpp
  ../src/isdbt-tuner/ISDBTProvider.cpp
  ../src/isdbt-tuner/RingBuffer.cpp
  
#  main
  ../src/main.cpp
)

add_dependencies(ginga nclua_build)

target_include_directories(ginga PRIVATE
  ../src

  ${CMAKE_BINARY_DIR}

  ${GLIB_INCLUDE_DIRS}
  ${GDKPIXBUF_INCLUDE_DIR}

  ${SDL2_INCLUDE_DIR}

  ${CAIRO_INCLUDE_DIRS}
  ${RSVG_INCLUDE_DIRS}
  ${LIBZIP_INCLUDE_DIRS}

  ${LinuxDVB_INCLUDE_DIRS}

  ${NCLUA_INCLUDE_DIRS}
)

target_link_libraries(ginga PRIVATE
  pthread
  jpeg

  XercesC::XercesC
  expat

  ${FFMPEG_LIBRARIES}
  swresample
  swscale
  avfilter

  ${SDL2_LIBRARY}
  ${SDL2_IMAGE_LIBRARIES}
  ${SDL2_TTF_LIBRARIES}
  
  ${CURL_LIBRARIES}
  ${GLIB_LIBRARIES}

  ${CAIRO_LIBRARIES}
  ${RSVG_LIBRARIES}
  ${LIBZIP_LIBRARY}

  nclua
)

# Copy datafiles to build dir
add_custom_target(copy-data-files ALL
  COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_SOURCE_DIR}/../data ${CMAKE_BINARY_DIR}/data
)

configure_file(${CMAKE_SOURCE_DIR}/config.h.in config.h)
