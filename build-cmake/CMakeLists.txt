cmake_minimum_required(VERSION 3.2)
project(ginga)

option (WITH_CEF "Build with chromium embedded support." OFF)
option (WITH_OPENGL "Build Ginga with opengl." ON)
option (WITH_RUNVIEW "Build nclcomposer's ginga plugin." OFF)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

# Defaul C++ params
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_INCLUDE_CURRENTDIR ON)
if (UNIX)
  add_definitions(-Wextra -Wall)
endif()

set (GINGA_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
set (GINGA_BINARY_TESTS_DIR ${CMAKE_BINARY_DIR}/tests)

# Find dependencies
find_package (GLIB 2.32 REQUIRED)

find_package (LibXml2 REQUIRED)
find_package (XercesC 2.7 REQUIRED)

find_package (GDKPixBuf REQUIRED)
find_package (GDK3 REQUIRED)

find_package (Cairo REQUIRED)
find_package (Pango REQUIRED)
find_package (RSVG REQUIRED)
find_package (GStreamer REQUIRED)
add_definitions (-DWITH_PANGO=1 -DWITH_GSTREAMER=1)

find_package (SDL2)
find_package (SDL2_image)
find_package (OpenGL)

if (OPENGL_FOUND)
  add_definitions (-DWITH_OPENGL=1)
else ()
  set (WITH_OPENGL OFF) # OpenGL not found, turn it off
endif()

find_package(GTK3)
if (GTK3_FOUND)
  set (WITH_GINGAGUI_GTK ON)
endif()

# NCLUA download/configure/build/install
include(ExternalProject)
ExternalProject_Add(nclua_build
  GIT_REPOSITORY "https://github.com/gflima/nclua.git"
  GIT_TAG master
  UPDATE_COMMAND ""

  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/nclua

  CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap &&
                    <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
                                           --without-nclua-gst

  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

ExternalProject_Get_Property(nclua_build install_dir)
set(NCLUA_INCLUDE_DIRS "${install_dir}/include")
link_directories("${install_dir}/lib")
#end NCLUA

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
  add_definitions (-DWITH_CEF=0)

endif()

set (LIBGINGA_SOURCES
  ../lib/ginga.h
  ../lib/aux-ginga.cpp
  ../lib/Ginga.cpp
  ../lib/GingaInternal.cpp

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

  ../lib/ncl/Action.cpp
  ../lib/ncl/Area.cpp
  ../lib/ncl/AreaLabeled.cpp
  ../lib/ncl/AssessmentStatement.cpp
  ../lib/ncl/AttributeAssessment.cpp
  ../lib/ncl/Base.cpp
  ../lib/ncl/Bind.cpp
  ../lib/ncl/CompositeRule.cpp
  ../lib/ncl/Composition.cpp
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
  ../lib/ncl/ParserXercesC.cpp
  ../lib/ncl/ParserLibXML.cpp
  ../lib/ncl/Port.cpp
  ../lib/ncl/Property.cpp
  ../lib/ncl/Refer.cpp
  ../lib/ncl/Region.cpp
  ../lib/ncl/RegionBase.cpp
  ../lib/ncl/Role.cpp
  ../lib/ncl/RuleBase.cpp
  ../lib/ncl/SimpleCondition.cpp
  ../lib/ncl/SimpleRule.cpp
  ../lib/ncl/Switch.cpp
  ../lib/ncl/SwitchPort.cpp
  ../lib/ncl/Transition.cpp
  ../lib/ncl/TransitionBase.cpp
  ../lib/ncl/TransitionUtil.cpp
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

set (LIBGINGA_INCLUDE_DIRS
  ${CMAKE_CURRENT_SOURCE_DIR}/../lib

  ${CMAKE_BINARY_DIR}

  ${LIBXML2_INCLUDE_DIR}
  ${GLIB_INCLUDE_DIRS}
  ${GDKPIXBUF_INCLUDE_DIRS}
  ${GDK3_INCLUDE_DIRS}

  ${CAIRO_INCLUDE_DIRS}
  ${PANGO_INCLUDE_DIRS}
  ${RSVG_INCLUDE_DIRS}
  ${GSTREAMER_INCLUDE_DIRS}
  ${GSTREAMER_BASE_INCLUDE_DIRS}

  ${NCLUA_INCLUDE_DIRS}
)

set (LIBGINGA_LIBS
  pthread

  ${XercesC_LIBRARIES}
  expat

  ${GLIB_LIBRARIES}

  ${LIBXML2_LIBRARIES}
  ${GDK3_LIBRARIES}
  ${GDKPIXBUF_LIBRARIES}

  ${CAIRO_LIBRARIES}
  ${PANGO_LIBRARIES}
  ${RSVG_LIBRARIES}
  ${GSTREAMER_LIBRARIES}
  ${GSTREAMER_APP_LIBRARIES}
  ${GSTREAMER_VIDEO_LIBRARIES}

  nclua
)

if (WITH_OPENGL)
  list (APPEND LIBGINGA_INCLUDE_DIRS ${OPENGL_INCLUDE_DIR})
  list (APPEND LIBGINGA_LIBS ${OPENGL_LIBRARIES})
endif ()

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

set (GINGAGUI_GTK_SOURCES
  ../src-gui/ginga-gtk.cpp
  ../src-gui/model/SaveLoadSettings.cpp
  ../src-gui/view/AboutWindow.cpp
  ../src-gui/view/BigPictureWindow.cpp
  ../src-gui/view/Draw.cpp
  ../src-gui/view/FullscreenWindow.cpp
  ../src-gui/view/HelpWindow.cpp
  ../src-gui/view/MainWindow.cpp
  ../src-gui/view/TvControlWindow.cpp
)

set (GINGAGUI_GTK_INCLUDE_DIRS
  ../src-gui/
  ../src-gui/model/
  ../src-gui/view/

  ${LIBGINGA_INCLUDE_DIRS}
  ${GTK3_INCLUDE_DIRS}
)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${GINGA_BINARY_DIR})

# libginga target
add_library (libginga STATIC ${LIBGINGA_SOURCES})
add_dependencies (libginga nclua_build)
target_include_directories (libginga PUBLIC ${LIBGINGA_INCLUDE_DIRS})
target_link_libraries (libginga PUBLIC ${LIBGINGA_LIBS})
set_target_properties (libginga PROPERTIES OUTPUT_NAME "ginga")

# ginga target
add_executable (ginga ../src/ginga.cpp)
target_include_directories (ginga PRIVATE ${GTK3_INCLUDE_DIRS})
target_link_libraries (ginga PRIVATE libginga ${GTK3_LIBRARIES})

# gingagl target
if (WITH_OPENGL)
  add_executable (gingagl ../src/ginga-gl.cpp)

  target_include_directories (gingagl PRIVATE
    ${SDL2_INCLUDE_DIR}
  )

  target_link_libraries (gingagl PRIVATE
    libginga
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
  )

endif ()

# gingagui target
if (WITH_GINGAGUI_GTK)
  add_executable (gingagui ${GINGAGUI_GTK_SOURCES})
  target_include_directories (gingagui PRIVATE ${GINGAGUI_GTK_INCLUDE_DIRS})
  target_link_libraries (gingagui PRIVATE libginga ${GTK3_LIBRARIES})
endif()

# nclcomposer's run-view target
if (WITH_RUNVIEW)
  set (CMAKE_INCLUDE_CURRENTDIR ON)
  set (CMAKE_AUTOMOC ON)
  set (CMAKE_AUTORCC ON)
  set (CMAKE_AUTOUIC ON)

  # Find the QtWidgets library
  find_package (Qt5 REQUIRED COMPONENTS Core Widgets)

  add_executable (runview
    ../src-runview/main.cpp
    ../src-runview/runview.cpp
    ../src-runview/runview.h

    ../src-runview/runview.ui
  )

  target_link_libraries (runview
    Qt5::Core
    Qt5::Widgets

    libginga
  )

  target_compile_definitions(runview PRIVATE -DQT_NO_KEYWORDS)

  target_include_directories(runview PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}

    ${LIBGINGA_INCLUDE_DIRS}
  )
endif ()

# Copy datafiles to build dir
add_custom_target (copy-data-files ALL
  COMMAND ${CMAKE_COMMAND} -E
    copy_directory ${CMAKE_SOURCE_DIR}/../data ${CMAKE_BINARY_DIR}/data
)

# Set rpath so that libraries can be placed next to the executable.
set_target_properties (ginga PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties (ginga PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)

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

enable_testing ()
add_custom_target (check COMMAND ${CMAKE_CTEST_COMMAND})
macro (add_ginga_test target)
  add_executable (${target} EXCLUDE_FROM_ALL ${ARGN})
  target_link_libraries (${target} PRIVATE libginga)
  set_target_properties (${target} PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                         ${GINGA_BINARY_TESTS_DIR})
  add_test (${target} tests/${target})
  add_dependencies (check ${target})
endmacro ()

file(GLOB GINGA_TESTS_SRC
  "../tests/*.cpp")

add_definitions (-DTOP_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}/../")
foreach (SRC ${GINGA_TESTS_SRC})
  get_filename_component (TEST_NAME ${SRC} NAME_WE)
  add_ginga_test (${TEST_NAME} ${SRC})
endforeach ()

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

Optional executables:
ginga-gui (gtk):	${WITH_GINGAGUI_GTK}
ginga-runview:          ${WITH_RUNVIEW}
ginga-gl:               ${WITH_OPENGL}
")
 
set (CPACK_PACKAGE_NAME ginga-idtv)
set (CPACK_PACKAGE_CONTACT robertogerson@telemidia.puc-rio.br)

install (DIRECTORY ${CMAKE_BINARY_OUTPUT_DIR} DESTINATION bin)
install (TARGETS ginga DESTINATION bin)
install (TARGETS gingagui DESTINATION bin)
include (CPack)

