cmake_minimum_required(VERSION 3.2)
project(ginga)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules)

# Defaul C++ params
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_INCLUDE_CURRENTDIR ON)
if (UNIX)
  add_definitions (-Wextra -Wall)
endif()

# Ginga-related cmake's variables
include (GNUInstallDirs)
set (PACKAGE "ginga")
set (GINGA_BINARY_DIR ${CMAKE_BINARY_DIR}/bin)
set (GINGA_BINARY_TESTS_DIR ${CMAKE_BINARY_DIR}/tests)
add_definitions (-DGINGADATADIR="${CMAKE_INSTALL_FULL_DATADIR}/${PACKAGE}")

# Find dependencies
find_package (GLIB 2.32 REQUIRED)

find_package (LibSoup 2.42.0)
find_package (LibXml2 REQUIRED)
find_package (GDKPixBuf REQUIRED)
find_package (GDK3 REQUIRED)

find_package (Cairo REQUIRED)
find_package (Pango REQUIRED)
find_package (RSVG REQUIRED)
find_package (GStreamer 1.0 REQUIRED)
add_definitions (-DWITH_PANGO=1 -DWITH_GSTREAMER=1)

find_package (Lua REQUIRED 5.2)

option (WITH_CEF "Build with chromium embedded support." OFF)
option (WITH_OPENGL "Build Ginga with opengl support." OFF)
option (WITH_GINGAQT "Build nclcomposer's ginga plugin." OFF)

if (WITH_OPENGL)
  find_package (SDL2)
  find_package (OpenGL)
  set (WITH_OPENGL ON)
else ()
  set (WITH_OPENGL OFF) # OpenGL not found, turn it off
endif()

find_package(GTK3)
if (GTK3_FOUND AND LIBSOUP_FOUND)
  set (WITH_GINGAGUI_GTK ON)
else()
  set (WITH_GINGAGUI_GTK OFF)
endif()

if (WITH_OPENGL)
  add_definitions (-DWITH_OPENGL=1)
endif ()

# NCLUA download/configure/build/install
include(ExternalProject)
ExternalProject_Add(nclua_build
  GIT_REPOSITORY "https://github.com/telemidia/nclua.git"
  GIT_TAG master
  UPDATE_COMMAND ""

  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/nclua

  CONFIGURE_COMMAND <SOURCE_DIR>/bootstrap &&
                    <SOURCE_DIR>/configure --prefix=<INSTALL_DIR>
                                           --without-nclua-gst

  BUILD_COMMAND make
  INSTALL_COMMAND make install
)

ExternalProject_Get_Property (nclua_build install_dir)
set (NCLUA_INCLUDE_DIRS "${install_dir}/include")
set (NCLUA_LIBRARY "${install_dir}/lib/libnclua.a")
link_directories ("${install_dir}/lib")
add_definitions (-DWITH_NCLUA=1)
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
  ../lib/aux-gl.cpp

  ../lib/Composition.cpp
  ../lib/Context.cpp
  ../lib/Document.cpp
  ../lib/Event.cpp
  ../lib/Formatter.cpp
  ../lib/Ginga.cpp
  ../lib/Media.cpp
  ../lib/MediaSettings.cpp
  ../lib/Object.cpp
  ../lib/Parser.cpp
  ../lib/ParserLua.cpp
  ../lib/Predicate.cpp
  ../lib/Switch.cpp

  ../lib/Player.cpp
  ../lib/PlayerAnimator.cpp
  ../lib/PlayerImage.cpp
  ../lib/PlayerLua.cpp
  ../lib/PlayerSigGen.cpp
  ../lib/PlayerText.cpp
  ../lib/PlayerVideo.cpp

#if WITH_LIBRSVG
  ../lib/PlayerSvg.cpp
#WITH_LIBRSVG
)

set (LIBGINGA_INCLUDE_DIRS
  ${CMAKE_BINARY_DIR}

  ${CMAKE_CURRENT_SOURCE_DIR}/../lib

  ${GLIB_INCLUDE_DIRS}

  ${LIBXML2_INCLUDE_DIR}
  ${GDKPIXBUF_INCLUDE_DIRS}
  ${GDK3_INCLUDE_DIRS}

  ${CAIRO_INCLUDE_DIRS}
  ${PANGO_INCLUDE_DIRS}
  ${RSVG_INCLUDE_DIRS}
  ${GSTREAMER_INCLUDE_DIRS}
  ${GSTREAMER_BASE_INCLUDE_DIRS}

  ${NCLUA_INCLUDE_DIRS}
  ${LUA_INCLUDE_DIR}
)

set (LIBGINGA_LIBS
  pthread

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

  ${NCLUA_LIBRARY}
  ${LUA_LIBRARIES}
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
endif ()

set (GINGAGUI_GTK_SOURCES
  ../src-gui/ginga-gtk.cpp
  ../src-gui/model/SaveLoadSettings.cpp
  ../src-gui/model/HttpMessage.cpp
  ../src-gui/view/TrackerWindow.cpp
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

# ginga-gl target
if (WITH_OPENGL)
  add_executable (ginga-gl ../src/ginga-gl.cpp)

  target_include_directories (ginga-gl PRIVATE
    ${SDL2_INCLUDE_DIR}
  )

  target_link_libraries (ginga-gl PRIVATE
    libginga
    ${SDL2_LIBRARY}
    ${SDL2_IMAGE_LIBRARIES}
  )
endif ()

# gingagui target
if (WITH_GINGAGUI_GTK)
  list (APPEND GINGAGUI_GTK_INCLUDE_DIRS
    ${LIBGINGA_INCLUDE_DIRS}
    ${LIBSOUP_INCLUDE_DIRS}
    ${GTK3_INCLUDE_DIRS}
  )

  set (GINGAGUI_GTK_LIBRARIES
    ${LIBSOUP_LIBRARIES}
    ${GTK3_LIBRARIES}
  )

  add_executable (gingagui ${GINGAGUI_GTK_SOURCES})
  target_include_directories (gingagui PRIVATE ${GINGAGUI_GTK_INCLUDE_DIRS})
  target_link_libraries (gingagui PRIVATE libginga ${GINGAGUI_GTK_LIBRARIES})
endif()

# ginga-qt executable.
if (WITH_GINGAQT)
  set (CMAKE_INCLUDE_CURRENTDIR ON)
  set (CMAKE_AUTOMOC ON)
  set (CMAKE_AUTORCC ON)
  set (CMAKE_AUTOUIC ON)

  # Find the QtWidgets library
  find_package (Qt5 REQUIRED COMPONENTS Core Widgets)

  add_executable (ginga-qt
    ../src/ginga-qt.cpp
  )

  target_link_libraries (ginga-qt
    Qt5::Core
    Qt5::Widgets

    libginga
  )

  target_compile_definitions(ginga-qt PRIVATE -DQT_NO_KEYWORDS)

  target_include_directories(ginga-qt PRIVATE
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

version:            ${VERSION}
install prefix:     ${CMAKE_INSTALL_PREFIX}
CMAKE_SYSTEM_NAME:  ${CMAKE_SYSTEM_NAME}
CMAKE_BUILD_TYPE:   ${CMAKE_BUILD_TYPE}
CMAKE_C_COMPILER:   ${CMAKE_CXX_COMPILER}
CMAKE_CXX_FLAGS:    ${CMAKE_CXX_FLAGS}

Optional dependencies:
with cef:           ${WITH_CEF}

Optional executables:
ginga-gui (gtk):    ${WITH_GINGAGUI_GTK}
ginga-qt:           ${WITH_GINGAQT}
ginga-gl:           ${WITH_OPENGL}
")

set (CPACK_PACKAGE_NAME ${PACKAGE})
set (CPACK_PACKAGE_CONTACT robertogerson@telemidia.puc-rio.br)

set (CPACK_DEBIAN_PACKAGE_DEPENDS
     "liblua5.3-0, libpangocairo-1.0-0, librsvg2-2,\
     libgstreamer1.0-0, libgstreamer-plugins-base1.0-0, \
     libgstreamer-plugins-good1.0-0, libgtk-3-0")

install (DIRECTORY ${CMAKE_BINARY_OUTPUT_DIR} DESTINATION bin)
install (TARGETS ginga DESTINATION bin)
if (WITH_GINGAGUI_GTK)
  install (TARGETS gingagui DESTINATION bin)
endif ()
install (DIRECTORY ../examples DESTINATION share/ginga/)

include (CPack)
