cmake_minimum_required(VERSION 3.2)
project(ginga)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENTDIR ON)

find_package(GLIB 2.32 REQUIRED)
find_package(CURL REQUIRED)

find_package(XercesC 2.7 REQUIRED)

find_package(Cairo REQUIRED)
find_package(Pango REQUIRED)
find_package(GDKPixBuf REQUIRED)
find_package(RSVG REQUIRED)
find_package(GStreamer REQUIRED)
add_definitions(-DWITH_PANGO=1 -DWITH_GSTREAMER=1)

find_package(GTK3)
if (GTK3_FOUND)
  set (WITH_GINGA_GTK OFF)
endif()

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

set (LIBGINGA_SOURCES
  ../lib/ginga-internal.cpp
  ../lib/ginga.cpp

  ../lib/formatter/Converter.cpp
  ../lib/formatter/ExecutionObject.cpp
  ../lib/formatter/ExecutionObjectContext.cpp
  ../lib/formatter/ExecutionObjectSwitch.cpp
  ../lib/formatter/ExecutionObjectSettings.cpp
  ../lib/formatter/NclActions.cpp
  ../lib/formatter/NclEvents.cpp
  ../lib/formatter/NclEventTransitionManager.cpp
  ../lib/formatter/NclFormatterLink.cpp
  ../lib/formatter/NclLinkAssessment.cpp
  ../lib/formatter/NclLinkCondition.cpp
  ../lib/formatter/NclLinkStatement.cpp
  ../lib/formatter/NclNodeNesting.cpp
  ../lib/formatter/RuleAdapter.cpp
  ../lib/formatter/Scheduler.cpp

  ../lib/mb/Display.cpp
  ../lib/mb/Dashboard.cpp

  ../lib/ncl/Area.cpp
  ../lib/ncl/AreaLabeled.cpp
  ../lib/ncl/AssessmentStatement.cpp
  ../lib/ncl/AttributeAssessment.cpp
  ../lib/ncl/Base.cpp
  ../lib/ncl/Bind.cpp
  ../lib/ncl/CompositeRule.cpp
  ../lib/ncl/Composition.cpp
  ../lib/ncl/CompoundAction.cpp
  ../lib/ncl/CompoundCondition.cpp
  ../lib/ncl/CompoundStatement.cpp
  ../lib/ncl/Connector.cpp
  ../lib/ncl/ConnectorBase.cpp
  ../lib/ncl/Context.cpp
  ../lib/ncl/Descriptor.cpp
  ../lib/ncl/DescriptorBase.cpp
  ../lib/ncl/Entity.cpp
  ../lib/ncl/EventUtil.cpp
  ../lib/ncl/Link.cpp
  ../lib/ncl/Media.cpp
  ../lib/ncl/NclDocument.cpp
  ../lib/ncl/Node.cpp
  ../lib/ncl/Parameter.cpp
  ../lib/ncl/Parser.cpp
  ../lib/ncl/Port.cpp
  ../lib/ncl/Property.cpp
  ../lib/ncl/Refer.cpp
  ../lib/ncl/Region.cpp
  ../lib/ncl/RegionBase.cpp
  ../lib/ncl/Role.cpp
  ../lib/ncl/RuleBase.cpp
  ../lib/ncl/SimpleAction.cpp
  ../lib/ncl/SimpleCondition.cpp
  ../lib/ncl/SimpleRule.cpp
  ../lib/ncl/Switch.cpp
  ../lib/ncl/SwitchPort.cpp
  ../lib/ncl/Transition.cpp
  ../lib/ncl/TransitionBase.cpp
  ../lib/ncl/TransitionUtil.cpp
  ../lib/ncl/TriggerExpression.cpp
  ../lib/ncl/ValueAssessment.cpp

  ../lib/player/ImagePlayer.cpp
  ../lib/player/LuaPlayer.cpp
  ../lib/player/Player.cpp
  ../lib/player/PlayerAnimator.cpp
  ../lib/player/TextPlayer.cpp
  ../lib/player/VideoPlayer.cpp

#if WITH_LIBRSVG
  ../lib/player/SvgPlayer.cpp
#WITH_LIBRSVG
)

set (GINGA_GUI_GTK_SOURCES
  ../src-gui/ginga-gtk.cpp
)

set (LIBGINGA_INCLUDE_DIRS
  ../lib

  ${CMAKE_BINARY_DIR}

  ${GLIB_INCLUDE_DIRS}
  ${GDKPIXBUF_INCLUDE_DIR}

  ${CAIRO_INCLUDE_DIRS}
  ${PANGO_INCLUDE_DIRS}
  ${RSVG_INCLUDE_DIRS}
  ${GSTREAMER_INCLUDE_DIRS}
  ${GSTREAMER_BASE_INCLUDE_DIRS}
  ${GTK3_INCLUDE_DIRS}

  ${LinuxDVB_INCLUDE_DIRS}

  ${NCLUA_INCLUDE_DIRS}
)

set (LIBGINGA_LIBS
  pthread

  ${XercesC_LIBRARIES}
  expat

  ${CURL_LIBRARIES}
  ${GLIB_LIBRARIES}
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_APP_LIBRARIES}
  ${GSTREAMER_VIDEO_LIBRARIES}
  ${GTK3_LIBRARIES}

  ${CAIRO_LIBRARIES}
  ${PANGO_LIBRARIES}
  ${RSVG_LIBRARIES}

  nclua
)

if (WITH_CEF)
  list (APPEND LIBGINGA_SOURCES
    ../lib/player/HTMLPlayer.cpp
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

add_library (libginga STATIC ${LIBGINGA_SOURCES})
add_dependencies (libginga nclua_build)
target_include_directories (libginga PUBLIC ${LIBGINGA_INCLUDE_DIRS})
target_link_libraries (libginga PUBLIC ${LIBGINGA_LIBS})
set_target_properties (libginga PROPERTIES OUTPUT_NAME "ginga")

add_executable (ginga ../src/ginga.cpp)
target_include_directories (ginga PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories (ginga PRIVATE ${LIBGINGA_INCLUDE_DIRS})
target_link_libraries (ginga PRIVATE libginga)


if (WITH_GINGA_GTK)
  add_executable (ginga-gtk ${GINGA_GUI_GTK_SOURCES})
  target_include_directories (ginga-gtk PRIVATE ${GINGA_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS})
  target_link_libraries (ginga-gtk PRIVATE ${GTK3_LIBRARIES} ${GINGA_LIBS})
  set_target_properties(ginga-gtk PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin/)
endif()

# Copy datafiles to build dir
add_custom_target (copy-data-files ALL
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

configure_file (${CMAKE_SOURCE_DIR}/config.h.in config.h)

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
with ginga-gtk:		${WITH_GINGA_GTK}
")
 
set (CPACK_PACKAGE_NAME ginga-idtv)
set (CPACK_PACKAGE_CONTACT robertogerson@telemidia.puc-rio.br)

install (DIRECTORY Release/ DESTINATION bin)
install (TARGETS ginga DESTINATION bin)
include (CPack)

