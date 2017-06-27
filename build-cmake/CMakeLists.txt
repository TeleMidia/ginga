cmake_minimum_required(VERSION 3.2)
project(ginga)

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
find_package(GStreamer REQUIRED)
add_definitions(-DWITH_PANGO=1 -DWITH_GSTREAMER=1)

# NCLUA download/configure/build/install
include(ExternalProject)
ExternalProject_Add(nclua_build
  GIT_REPOSITORY "https://github.com/gflima/nclua.git"
  GIT_TAG master
  UPDATE_COMMAND ""

  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/nclua

  CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap && <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> --without-nclua-gst
  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

ExternalProject_Get_Property(nclua_build install_dir)
set(NCLUA_INCLUDE_DIRS "${install_dir}/include")
link_directories("${install_dir}/lib")
#end NCLUA

option (WITH_CEF "Build with chromium embedded support" ON)

if (WITH_CEF) # Download, link, build with CEF
  set(CEF_VERSION cef_binary_3.3029.1619.geeeb5d7_linux64_minimal)
  set(CEF_SHA1 f809ce65b0b532fcbba6cec5f0e60f731fd3cbbd)
  set(CEF_FILE ${CEF_VERSION}.tar.bz2)
  set(CEF_URL http://opensource.spotify.com/cefbuilds/${CEF_FILE})
  set(CEF_LOCAL_PATH ${CMAKE_BINARY_DIR}/3rdparty)
  set(CEF_LOCAL_FILE ${CMAKE_BINARY_DIR}/3rdparty/${CEF_FILE}.tar.gz)

  # Download chromium embedded files
  if (NOT EXISTS "${CEF_LOCAL_FILE}")
    message(STATUS "-- Downloading chromium embedded binary package...")
    file(DOWNLOAD ${CEF_URL} ${CEF_LOCAL_FILE}
         EXPECTED_HASH SHA1=${CEF_SHA1}
         SHOW_PROGRESS)

    message(STATUS "-- Extracting chromium embedded files...")
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E tar xzf ${CEF_LOCAL_FILE}
      WORKING_DIRECTORY ${CEF_LOCAL_PATH}
    )
  endif()

  set(CEF_ROOT "${CEF_LOCAL_PATH}/${CEF_VERSION}/")
  find_package(CEF REQUIRED)
  add_definitions(-DWITH_CEF=1)
  add_subdirectory(${CEF_ROOT}/libcef_dll libcef_dll_bin)

  # Logical target used to link the libcef library.
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
  SET (CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/bin/")

else()
  add_definitions(-DWITH_CEF=0)

endif()

set (GINGA_SOURCES
  ../src/ginga.h

  ../src/formatter/Converter.cpp
  ../src/formatter/ExecutionObject.cpp
  ../src/formatter/ExecutionObjectContext.cpp
  ../src/formatter/ExecutionObjectSwitch.cpp
  ../src/formatter/NclActions.cpp
  ../src/formatter/NclCascadingDescriptor.cpp
  ../src/formatter/NclEvents.cpp
  ../src/formatter/NclEventTransitionManager.cpp
  ../src/formatter/NclFormatterLink.cpp
  ../src/formatter/NclFormatterRegion.cpp
  ../src/formatter/NclLinkAssessmentStatement.cpp
  ../src/formatter/NclLinkAttributeAssessment.cpp
  ../src/formatter/NclLinkCompoundStatement.cpp
  ../src/formatter/NclLinkCondition.cpp
  ../src/formatter/NclLinkValueAssessment.cpp
  ../src/formatter/NclNodeNesting.cpp
  ../src/formatter/Parser.cpp
  ../src/formatter/RuleAdapter.cpp
  ../src/formatter/Settings.cpp
  ../src/formatter/Scheduler.cpp
  ../src/mb/Display.cpp
  ../src/mb/Dashboard.cpp
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
  ../src/ncl/Entity.cpp
  ../src/ncl/EventUtil.cpp
  ../src/ncl/FocusDecoration.cpp
  ../src/ncl/IntervalAnchor.cpp
  ../src/ncl/KeyNavigation.cpp
  ../src/ncl/LabeledAnchor.cpp
  ../src/ncl/LambdaAnchor.cpp
  ../src/ncl/LayoutRegion.cpp
  ../src/ncl/Link.cpp
  ../src/ncl/NclDocument.cpp
  ../src/ncl/Node.cpp
  ../src/ncl/NodeEntity.cpp
  ../src/ncl/Parameter.cpp
  ../src/ncl/Port.cpp
  ../src/ncl/PropertyAnchor.cpp
  ../src/ncl/RectangleSpatialAnchor.cpp
  ../src/ncl/ReferNode.cpp
  ../src/ncl/ReferenceContent.cpp
  ../src/ncl/ReferredNode.cpp
  ../src/ncl/RegionBase.cpp
  ../src/ncl/Role.cpp
  ../src/ncl/Rule.cpp
  ../src/ncl/RuleBase.cpp
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
  ../src/player/ImagePlayer.cpp
  ../src/player/LuaPlayer.cpp
  ../src/player/Player.cpp
  ../src/player/PlayerAnimator.cpp
  ../src/common-color-table.cpp
  ../src/common-key-table.cpp
  ../src/common-mime-table.cpp
  ../src/common.cpp

#if WITH_GSTREAMER
  ../src/player/VideoPlayer.cpp
# WITH_GSTREAMER

#if WITH_LIBRSVG
  ../src/player/SvgPlayer.cpp
#WITH_LIBRSVG

#if WITH_PANGO
  ../src/player/TextPlayer.cpp
# WITH_PANGO

#  main
  ../src/main.cpp
)

set (GINGA_INCLUDE_DIRS
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
)

set (GINGA_LIBS
  pthread

  ${XercesC_LIBRARIES}
  expat

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
)

if (WITH_CEF)
  list (APPEND GINGA_SOURCES
    ../src/player/HTMLPlayer.cpp
  )
  list (APPEND GINGA_INCLUDE_DIRS
    ${CEF_ROOT}
    ${CEF_ROOT}/include
  )
  list (APPEND GINGA_LIBS
    libcef_lib
    libcef_dll_wrapper
    ${CEF_STANDARD_LIBS}
  )
endif()

add_executable(ginga ${GINGA_SOURCES})
add_dependencies(ginga nclua_build)
target_include_directories(ginga PRIVATE ${GINGA_INCLUDE_DIRS})
target_link_libraries(ginga PRIVATE ${GINGA_LIBS})

# Copy datafiles to build dir
add_custom_target(copy-data-files ALL
  COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_SOURCE_DIR}/../data ${CMAKE_BINARY_DIR}/data
)

# Set rpath so that libraries can be placed next to the executable.
set_target_properties(ginga PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(ginga PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(ginga PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/)

if (WITH_CEF)
  # Copy binary and resource files to the target output directory.
  COPY_FILES(ginga "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
  COPY_FILES(ginga "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

  # Set SUID permissions on the chrome-sandbox target.
  SET_LINUX_SUID_PERMISSIONS(ginga "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()

configure_file(${CMAKE_SOURCE_DIR}/config.h.in config.h)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  set (top_builddir ${CMAKE_CURRENT_BINARY_DIR})
  set (top_srcdir ${CMAKE_CURRENT_SOURCE_DIR}/..)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../doc/Doxyfile.in
                 ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_BINARY_DIR}/doc/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

message ( "
---
summary of main build options:

version:		${VERSION}
install prefix:		${CMAKE_INSTALL_PREFIX}
CMAKE_SYSTEM_NAME:	${CMAKE_SYSTEM_NAME}
CMAKE_BUILD_TYPE:	${CMAKE_BUILD_TYPE}
CMAKE_C_COMPILER:	${CMAKE_CXX_COMPILER}
CMAKE_CXX_FLAGS:	${CMAKE_CXX_FLAGS}

Optional dependencies:
with cef:		${WITH_CEF}
")

 
set (CPACK_PACKAGE_NAME ginga-idtv)
set (CPACK_PACKAGE_CONTACT robertogerson@telemidia.puc-rio.br)

install (DIRECTORY Release/ DESTINATION bin)
install(TARGETS ginga DESTINATION bin)
include (CPack)

