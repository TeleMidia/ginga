cmake_minimum_required(VERSION 3.2)
project(ginga)

add_definitions( -fexceptions )

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENTDIR ON)

find_package(GLIB 2.32 REQUIRED)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(CURL REQUIRED)

find_package(XercesC 2.7 REQUIRED)

find_package(Cairo REQUIRED)
find_package(Pango REQUIRED)
find_package(GDKPixBuf REQUIRED)
find_package(RSVG REQUIRED)
find_package(FFmpeg REQUIRED)
find_package(GStreamer REQUIRED)

set (CEF_ROOT "${CMAKE_SOURCE_DIR}/../deps/cef")
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")
find_package(CEF REQUIRED)
add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper)
# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
SET_CEF_TARGET_OUT_DIR()

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

  CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap && <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --without-nclua-gst
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
  ../src/formatter/AdapterApplicationPlayer.cpp
  ../src/formatter/AdapterFormatterPlayer.cpp
  ../src/formatter/AdapterImagePlayer.cpp
  ../src/formatter/AdapterLuaPlayer.cpp
  ../src/formatter/AdapterNCLPlayer.cpp
  ../src/formatter/AdapterPlayerManager.cpp
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
  ../src/formatter/PresentationContext.cpp
  ../src/formatter/PrivateBaseContext.cpp
  ../src/formatter/RuleAdapter.cpp
  ../src/mb/Display.cpp
  ../src/mb/DisplayDebug.cpp
  ../src/mb/KeyMap.cpp
  ../src/mb/SDLWindow.cpp
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
  ../src/player/ImagePlayer.cpp
  ../src/player/LuaPlayer.cpp
  ../src/player/Player.cpp
  ../src/player/PlayerAnimator.cpp
  ../src/system/Thread.cpp
  ../src/util/functions.cpp

#if WITH_ESPEAK
#  ../src/formatter/AdapterSsmlPlayer.cpp
#  ../src/player/SsmlPlayer.cpp
# WITH_ESPEAK

#if WITH_GSTREAMER
  ../src/formatter/AdapterVideoPlayer.cpp
  ../src/player/VideoPlayer.cpp
# WITH_GSTREAMER

#if WITH_LIBRSVG
  ../src/formatter/AdapterSvgPlayer.cpp
  ../src/player/SvgPlayer.cpp
#WITH_LIBRSVG

#if WITH_PANGO
  ../src/formatter/AdapterTextPlayer.cpp
  ../src/player/TextPlayer.cpp
# WITH_PANGO

#if WITH_CEF
  ../src/formatter/AdapterHTMLPlayer.cpp
  ../src/player/HTMLPlayer.cpp
# WITH_CEF

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
  ${PANGO_INCLUDE_DIRS}
  ${RSVG_INCLUDE_DIRS}
  ${GSTREAMER_INCLUDE_DIRS}
  ${GSTREAMER_BASE_INCLUDE_DIRS}

  ${LinuxDVB_INCLUDE_DIRS}

  ${NCLUA_INCLUDE_DIRS}
  "${CEF_ROOT}/include"
)

target_link_libraries(ginga PRIVATE
  pthread

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
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_APP_LIBRARIES}
  ${GSTREAMER_VIDEO_LIBRARIES}

  ${CAIRO_LIBRARIES}
  ${PANGO_LIBRARIES}
  ${RSVG_LIBRARIES}

  nclua

  libcef_lib
  libcef_dll_wrapper
  ${CEF_STANDARD_LIBS}
)

# Copy datafiles to build dir
add_custom_target(copy-data-files ALL
  COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_SOURCE_DIR}/../data ${CMAKE_BINARY_DIR}/data
)

configure_file(${CMAKE_SOURCE_DIR}/config.h.in config.h)

# Copy binary and resource files to the target output directory.
COPY_FILES(ginga "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
COPY_FILES(ginga "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

# Set SUID permissions on the chrome-sandbox target.
SET_LINUX_SUID_PERMISSIONS(ginga "${CEF_TARGET_OUT_DIR}/chrome-sandbox")

